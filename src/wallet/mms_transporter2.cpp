#include "mms_transporter2.h"
#include "string_coding.h"
#include <boost/format.hpp>
#include "wallet_errors.h"
#include "wallet2.h"
#include "net/http_client.h"
#include "net/net_parse_helpers.h"
#include "net/abstract_http_client.h"
#include <memory>
#include <algorithm>
#include <msgpack.hpp>

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "wallet.mms"

#define MMS_LAYER 2

namespace mms
{

struct data_version_t
{
  uint8_t major, minor;
  MSGPACK_DEFINE(major, minor)
};

struct data_cipher_t
{
  std::string data;
  crypto::chacha_iv iv;
  union { crypto::public_key k; unsigned char r[32]; } encryption_public_key;
  MSGPACK_DEFINE(data, iv.data, encryption_public_key.r)
};

mms_transporter2::mms_transporter2(tools::wallet2 *wallet)
  : m_wallet(wallet)
  , callback_(wallet->callback())
{
  m_wallet->callback(this);
}

bool mms_transporter2::receive_messages(const std::vector<cryptonote::account_public_address> &addresses,
                                           std::vector<transport_message> &messages)
{
  bool rc = false;
  for(const auto& m : messages_)
    if (std::find(addresses.begin(), addresses.end(), m.source_monero_address) != addresses.end())
    {
      messages.push_back(m); rc = true;
    }
  return rc;
}

bool mms_transporter2::send_message(const transport_message &message)
{
  std::string data = epee::string_encoding::base64_encode(
              epee::serialization::store_t_to_json(message));
  data_cipher_t cipher;
  try
  {
    encrypt(
      message.destination_monero_address.m_view_public_key,
      data,
      cipher.data,
      cipher.encryption_public_key.k,
      cipher.iv);
  }
  catch (const std::exception& e)
  {
    MLOG_RED(el::Level::Warning, "failed encript message for store: " << e.what());
    return false;
  }

  data_version_t ver{0,1};
  std::stringstream ss;
  msgpack::pack(ss, ver);
  msgpack::pack(ss, cipher);

  std::vector<uint8_t> extra;
  cryptonote::tx_extra_sms em{MMS_LAYER, ss.str()};
  cryptonote::set_sms_to_extra(extra, em);

  cryptonote::tx_destination_entry de;
  de.amount = COIN;
  de.addr = message.destination_monero_address;
  de.is_integrated = false;
  de.is_subaddress = false;

  std::vector<cryptonote::tx_destination_entry> dsts;
  dsts.push_back(de);

  try
  {
    std::vector<tools::wallet2::pending_tx> ptx_vector =
      m_wallet->create_transactions_2(
        dsts,
        0 /* fake_outs_count */,
        0 /* unlock_time */,
        0 /* priority */,
        extra,
        0 /* m_current_subaddress_account */,
        {}
      );

    for(auto& ptx : ptx_vector)
      m_wallet->commit_tx(ptx);
  }
  catch(const std::exception& e)
  {
    MWARNING("error create tx: " << e.what());
    return false;
  }

  return true;
}

bool mms_transporter2::delete_message(const std::string &transport_id)
{
  for(auto it = messages_.begin(); it<messages_.end(); it++)
    if(it->transport_id == transport_id) {
      messages_.erase(it);
      return true;
    }
  return false;
}

void mms_transporter2::on_received(const std::string& data)
{
  data_version_t  ver;
  data_cipher_t   cipher;

  try
  {
    size_t off = 0;
    msgpack::object_handle v0 = msgpack::unpack(data.data(), data.size(), off);
    v0.get().convert(ver);
    
    if(ver.major != 0 || ver.minor != 1)
    {
      MLOG_RED(el::Level::Warning, "Error version of message, ver: " << ver.major << "." << ver.minor);
      return;
    }

    msgpack::object_handle o1 = msgpack::unpack(data.data(), data.size(), off);
    o1.get().convert(cipher);
  }
  catch (const std::exception& e)
  {
    MLOG_RED(el::Level::Warning, "Error deserialize: " << e.what());
    return;
  }

  std::string m;

  try
  {
    decrypt(
      cipher.data,
      cipher.encryption_public_key.k,
      cipher.iv,
      m_wallet->get_account().get_keys().m_view_secret_key,
      m);
  }
  catch (const std::exception& e)
  {
    MLOG_RED(el::Level::Warning, "Error decrypt: " << e.what());
    return;
  }

  transport_message message;
  if (!epee::serialization::load_t_from_json(message,
                epee::string_encoding::base64_decode(m)))
  {
    MLOG_RED(el::Level::Warning, "Failed to deserialize message");
    return;
  }

  messages_.push_back(message);
}

void mms_transporter2::on_new_block(uint64_t height, const cryptonote::block& block) { if(callback_) callback_->on_new_block(height, block); }
void mms_transporter2::on_money_received(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& tx, uint64_t amount, const cryptonote::subaddress_index& subaddr_index, bool is_change, uint64_t unlock_time) { if(callback_) callback_->on_money_received(height, txid, tx, amount, subaddr_index, is_change, unlock_time); }
void mms_transporter2::on_sms_received(uint64_t /*height*/, const crypto::hash &/*txid*/, const cryptonote::transaction& /*tx*/, uint32_t layer, const std::string& data, uint64_t /*amount*/, const cryptonote::subaddress_index& /*subaddr_index*/) { if(layer == MMS_LAYER) on_received(data); }
void mms_transporter2::on_unconfirmed_money_received(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& tx, uint64_t amount, const cryptonote::subaddress_index& subaddr_index) { if(callback_) callback_->on_unconfirmed_money_received(height, txid, tx, amount, subaddr_index); }
void mms_transporter2::on_money_spent(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& in_tx, uint64_t amount, const cryptonote::transaction& spend_tx, const cryptonote::subaddress_index& subaddr_index) { if(callback_) callback_->on_money_spent(height, txid, in_tx, amount, spend_tx, subaddr_index); }
void mms_transporter2::on_skip_transaction(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& tx) { if(callback_) callback_->on_skip_transaction(height, txid, tx); }
boost::optional<epee::wipeable_string> mms_transporter2::on_get_password(const char *reason) {  if(callback_) callback_->on_get_password(reason); return boost::none; }
void mms_transporter2::on_lw_new_block(uint64_t height) { if(callback_) callback_->on_lw_new_block(height); }
void mms_transporter2::on_lw_money_received(uint64_t height, const crypto::hash &txid, uint64_t amount) { if(callback_) callback_->on_lw_money_received(height, txid, amount); }
void mms_transporter2::on_lw_unconfirmed_money_received(uint64_t height, const crypto::hash &txid, uint64_t amount) { if(callback_) callback_->on_lw_unconfirmed_money_received(height, txid, amount); }
void mms_transporter2::on_lw_money_spent(uint64_t height, const crypto::hash &txid, uint64_t amount) { if(callback_) callback_->on_lw_money_spent(height, txid, amount); }
void mms_transporter2::on_device_button_request(uint64_t code) { if(callback_) callback_->on_device_button_request(code); }
void mms_transporter2::on_device_button_pressed() { if(callback_) callback_->on_device_button_pressed(); }
boost::optional<epee::wipeable_string> mms_transporter2::on_device_pin_request() {  if(callback_) return callback_->on_device_pin_request(); return boost::none; }
boost::optional<epee::wipeable_string> mms_transporter2::on_device_passphrase_request(bool & on_device) { if(callback_) return callback_->on_device_passphrase_request(on_device); on_device = true; return boost::none; }
void mms_transporter2::on_device_progress(const hw::device_progress& event) { if(callback_) callback_->on_device_progress(event); };
void mms_transporter2::on_pool_tx_removed(const crypto::hash &txid) { if(callback_) callback_->on_pool_tx_removed(txid); }

void mms_transporter2::encrypt(crypto::public_key public_key, const std::string &plaintext,
                            std::string &ciphertext, crypto::public_key &encryption_public_key, crypto::chacha_iv &iv)
{
  crypto::secret_key encryption_secret_key;
  crypto::generate_keys(encryption_public_key, encryption_secret_key);

  crypto::key_derivation derivation;
  bool success = crypto::generate_key_derivation(public_key, encryption_secret_key, derivation);
  THROW_WALLET_EXCEPTION_IF(!success, tools::error::wallet_internal_error, "Failed to generate key derivation for message encryption");

  crypto::chacha_key chacha_key;
  crypto::generate_chacha_key(&derivation, sizeof(derivation), chacha_key, 1);
  iv = crypto::rand<crypto::chacha_iv>();
  ciphertext.resize(plaintext.size());
  crypto::chacha20(plaintext.data(), plaintext.size(), chacha_key, iv, &ciphertext[0]);
}

void mms_transporter2::decrypt(const std::string &ciphertext, const crypto::public_key &encryption_public_key, const crypto::chacha_iv &iv,
                            const crypto::secret_key &view_secret_key, std::string &plaintext)
{
  crypto::key_derivation derivation;
  bool success = crypto::generate_key_derivation(encryption_public_key, view_secret_key, derivation);
  THROW_WALLET_EXCEPTION_IF(!success, tools::error::wallet_internal_error, "Failed to generate key derivation for message decryption");
  crypto::chacha_key chacha_key;
  crypto::generate_chacha_key(&derivation, sizeof(derivation), chacha_key, 1);
  plaintext.resize(ciphertext.size());
  crypto::chacha20(ciphertext.data(), ciphertext.size(), chacha_key, iv, &plaintext[0]);
}

}
