#ifndef __mms_transporter_h__
#define __mms_transporter_h__

#include "i_wallet2_callback.h"
#include "mms_transporter_message.h"
#include "serialization/keyvalue_serialization.h"
#include "cryptonote_basic/cryptonote_basic.h"
#include "cryptonote_basic/cryptonote_boost_serialization.h"
#include "cryptonote_basic/account_boost_serialization.h"
#include "cryptonote_basic/cryptonote_basic.h"
#include "common/util.h"
#include "wipeable_string.h"
#include <vector>

namespace tools { class wallet2; }

namespace mms
{

class mms_transporter2 : public tools::i_wallet2_callback
{
public:
  mms_transporter2(tools::wallet2 *wallet);
  bool send_message(const transport_message &message);
  bool receive_messages(const std::vector<cryptonote::account_public_address> &addresses,
                        std::vector<transport_message> &messages);
  bool delete_message(const std::string &transport_id);

  void on_received(const std::string& data);

  // i_wallet2_callback
  virtual void on_new_block(uint64_t height, const cryptonote::block& block);
  virtual void on_money_received(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& tx, uint64_t amount, const cryptonote::subaddress_index& subaddr_index, bool is_change, uint64_t unlock_time);
  virtual void on_sms_received(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& tx, uint32_t layer, const std::string& data, uint64_t amount, const cryptonote::subaddress_index& subaddr_index);
  virtual void on_unconfirmed_money_received(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& tx, uint64_t amount, const cryptonote::subaddress_index& subaddr_index);
  virtual void on_money_spent(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& in_tx, uint64_t amount, const cryptonote::transaction& spend_tx, const cryptonote::subaddress_index& subaddr_index);
  virtual void on_skip_transaction(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& tx);
  virtual boost::optional<epee::wipeable_string> on_get_password(const char *reason);
  virtual void on_lw_new_block(uint64_t height);
  virtual void on_lw_money_received(uint64_t height, const crypto::hash &txid, uint64_t amount);
  virtual void on_lw_unconfirmed_money_received(uint64_t height, const crypto::hash &txid, uint64_t amount);
  virtual void on_lw_money_spent(uint64_t height, const crypto::hash &txid, uint64_t amount);
  virtual void on_device_button_request(uint64_t code);
  virtual void on_device_button_pressed();
  virtual boost::optional<epee::wipeable_string> on_device_pin_request();
  virtual boost::optional<epee::wipeable_string> on_device_passphrase_request(bool & on_device);
  virtual void on_device_progress(const hw::device_progress& event);
  virtual void on_pool_tx_removed(const crypto::hash &txid);

private:
  static void encrypt(crypto::public_key public_key, const std::string &plaintext,
    std::string &ciphertext, crypto::public_key &encryption_public_key, crypto::chacha_iv &iv);
  static void decrypt(const std::string &ciphertext, const crypto::public_key &encryption_public_key, const crypto::chacha_iv &iv,
    const crypto::secret_key &view_secret_key, std::string &plaintext);

  tools::wallet2 *m_wallet;
  tools::i_wallet2_callback* callback_;
  std::vector<transport_message> messages_;
};

}

#endif
