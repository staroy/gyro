#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <chrono>

#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>

#include "string_tools.h"
#include "misc_log_ex.h"
#include "cryptonote_config.h"
#include "wallet2.h"

#include "wallet_zyre.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "wallet.sms"

#define SMS_LAYER 1

namespace zyre { namespace wallet {

  struct data_version_t
  {
    uint8_t major, minor;
    MSGPACK_DEFINE(major, minor)
  };

  struct data_sign_t
  {
    std::string d;

    union {
      crypto::signature data;
      unsigned char r[64];
      MSGPACK_DEFINE(r)
    } s;

    std::string n, // name e.t.c. label
                a; // account public address

    MSGPACK_DEFINE_MAP(d, s, n, a)
  };
  
  server::server(tools::wallet2 *w_ptr, const std::string& path)
    : w_ptr_(w_ptr)
    , path_(path)
    , callback_(w_ptr->callback())
    , zsrv_(w_ptr->get_address_as_str()+".srv", {w_ptr->zgroup()}, w_ptr->znet() + w_ptr->zpin())
    , zyre_(ios_, w_ptr->get_address_as_str(), zsrv_)
    , me_(w_ptr->get_address_as_str())
    , rpc_(w_ptr)
    , on_sms_receive_(nullptr)
  {
    w_ptr_->callback(this);

    zyre_.on_r("sms_label", zyre::fwrap_r(&server::sms_label, this));
    zyre_.on_r("sms_addr_book", zyre::fwrap_r(&server::sms_addr_book, this));
    zyre_.on_r("sms_count", zyre::fwrap_r(&server::sms_count, this));
    zyre_.on_r("sms_array", zyre::fwrap_r(&server::sms_array, this));
    zyre_.on_r("sms_get", zyre::fwrap_r(&server::sms_get, this));
    zyre_.on_r("sms_put", zyre::fwrap_r(&server::sms_put, this));

    rpc_.init(zyre_);
  }

  void server::start()
  {
    zsrv_.start();
    zyre_.start();
    zyre_.s_join(w_ptr_->get_address_as_str());
    zyre_.join(w_ptr_->zgroup());
    thr_ = std::thread([&](){ ios_.run(); });
    MLOG_GREEN(el::Level::Warning, "Wallet ZYRE started.");
  }

  void server::stop()
  {
    zyre_.stop();
    ios_.stop();
    thr_.join();
    zsrv_.stop();
    zsrv_.join();
    MLOG_GREEN(el::Level::Warning, "Wallet ZYRE stopped.");
  }

  std::string server::_sms_label()
  {
    return w_ptr_->get_subaddress_label({0,0});
  }

  void server::sms_label(zyre::resp_t r)
  {
    r(me_, _sms_label());
  }

  void server::sms_addr_book(zyre::resp_t r)
  {
    contacts_t cc;
    for(const auto& a : w_ptr_->get_address_book())
    {
      cc.push_back({
        cryptonote::get_account_address_as_str(w_ptr_->nettype(), a.m_is_subaddress, a.m_address),
        a.m_label
      });
    }
    r(me_, cc);
  }

  void server::sms_count(const std::string& from, zyre::resp_t r)
  {
    r(me_, _sms_count(from));
  }

  uint64_t server::_sms_count(const std::string& from)
  {
    std::string tmp = from + salt_;
    hash_t h_from;
    crypto::cn_fast_hash(tmp.data(), tmp.size(), h_from.u.h);
    uint64_t n = 0;
    data_[CONTACT][h_from][MESSAGES_COUNTER] >> n;
    return n;
  }

  void server::sms_array(const std::string& from, const uint64_t& n, const uint64_t& c, zyre::resp_t r)
  {
    std::vector<std::string> sms;
    std::vector<bool> sended;
    std::vector<uint64_t> ts;
    _sms_array(from, n, c, sms, sended, ts);
    r(me_, sms, sended, ts);
  }

  void server::_sms_array(
        const std::string& from,
        const uint64_t& n,
        const uint64_t& cnt,
        std::vector<std::string>& sms,
        std::vector<bool>& sended,
        std::vector<uint64_t>& ts)
  {
    std::string self = w_ptr_->get_address_as_str();
    std::string tmp = from + salt_;
    hash_t h_from;
    crypto::cn_fast_hash(tmp.data(), tmp.size(), h_from.u.h);
    lldb::cxx::db mm = data_[MESSAGE][h_from];
    uint64_t c = 0;
    for(auto it = mm.find(n); c < cnt && it != mm.end(); it++, c++)
    {
      uint64_t _n = 0;
      it->first >> _n;

      data_cipher_t cipher;
      it->second >> cipher;

      std::string buf;
      try
      {
        zyre::wallet::decrypt(cipher.d, cipher.u.k, cipher.iv, sec_, buf);
      }
      catch (const std::exception& e)
      {
        MLOG_RED(el::Level::Warning, "error decrypt: " << e.what());
        return;
      }

      try
      {
        data_t info;
        msgpack::object_handle o1 = msgpack::unpack(buf.data(), buf.size());
        o1.get().convert(info);
        sms.push_back(info.d);
        ts.push_back(info.ts);
        //if(f == MESSAGE_RECEIVED)
        if(info.fa == self)
          sended.push_back(true);
        else
          sended.push_back(false);
      }
      catch (const std::exception& e)
      {
        MLOG_RED(el::Level::Warning, "error deserialize: " << e.what());
        return;
      }
    }
  }

  void server::_sms_array(
        const uint64_t& n,
        const uint64_t& cnt,
        std::vector<std::string>& from,
        std::vector<std::string>& sms,
        std::vector<uint64_t>& ts)
  {
    std::string self = w_ptr_->get_address_as_str();
    std::string tmp = self + salt_;
    hash_t h_to;
    crypto::cn_fast_hash(tmp.data(), tmp.size(), h_to.u.h);
    lldb::cxx::db mm = data_[MESSAGE][h_to];
    uint64_t c = 0;
    for(auto it = mm.find(n); c < cnt && it != mm.end(); it++, c++)
    {
      uint64_t _n = 0;
      it->first >> _n;

      data_cipher_t cipher;
      it->second >> cipher;

      std::string buf;
      try
      {
        zyre::wallet::decrypt(cipher.d, cipher.u.k, cipher.iv, sec_, buf);
      }
      catch (const std::exception& e)
      {
        MLOG_RED(el::Level::Warning, "error decrypt: " << e.what());
        return;
      }

      try
      {
        data_t info;
        msgpack::object_handle o1 = msgpack::unpack(buf.data(), buf.size());
        o1.get().convert(info);
        sms.push_back(info.d);
        from.push_back(info.f);
        ts.push_back(info.ts);
      }
      catch (const std::exception& e)
      {
        MLOG_RED(el::Level::Warning, "error deserialize: " << e.what());
        return;
      }
    }
  }

  void server::sms_get(const std::string& from, const uint64_t& n, zyre::resp_t r)
  {
    std::string sms; bool sended; uint64_t ts;
    _sms_get(from, n, sms, sended, ts);
    r(me_, sms, sended, ts);
  }

  void server::_sms_get(const std::string& from, const uint64_t& n, std::string& sms, bool& sended, uint64_t& ts)
  {
    std::string tmp = from + salt_;
    hash_t h_from;
    crypto::cn_fast_hash(tmp.data(), tmp.size(), h_from.u.h);
    lldb::cxx::db m = data_[MESSAGE][h_from];
    auto it = m.find(n);
    if(it != m.end())
    {
      uint64_t _n = 0; hash_t h_to; uint64_t f = 0;
      it->first >> _n >> h_to >> f;

      data_cipher_t cipher;
      it->second >> cipher;

      std::string buf;
      try
      {
        zyre::wallet::decrypt(cipher.d, cipher.u.k, cipher.iv, sec_, buf);
      }
      catch (const std::exception& e)
      {
        MLOG_RED(el::Level::Warning, "error decrypt: " << e.what());
        return;
      }

      try
      {
        msgpack::object_handle o1 = msgpack::unpack(buf.data(), buf.size());
        data_t info;
        o1.get().convert(info);
        sms = info.d;
        ts = info.ts;
        if(f == MESSAGE_RECEIVED)
          sended = false;
        else
          sended = true;
      }
      catch (const std::exception& e)
      {
        MLOG_RED(el::Level::Warning, "error deserialize: " << e.what());
        return;
      }
    }
  }

  void server::_sms_get(const uint64_t& n, std::string& from, std::string& sms, uint64_t& ts)
  {
    std::string tmp = w_ptr_->get_address_as_str() + salt_;
    hash_t h_to;
    crypto::cn_fast_hash(tmp.data(), tmp.size(), h_to.u.h);
    lldb::cxx::db m = data_[MESSAGE][h_to];
    auto it = m.find(n);
    if(it != m.end())
    {
      uint64_t _n = 0;
      it->first >> _n;

      data_cipher_t cipher;
      it->second >> cipher;

      std::string buf;
      try
      {
        zyre::wallet::decrypt(cipher.d, cipher.u.k, cipher.iv, sec_, buf);
      }
      catch (const std::exception& e)
      {
        MLOG_RED(el::Level::Warning, "error decrypt: " << e.what());
        return;
      }

      try
      {
        msgpack::object_handle o1 = msgpack::unpack(buf.data(), buf.size());
        data_t info;
        o1.get().convert(info);
        sms = info.d;
        from = info.f;
        ts = info.ts;
      }
      catch (const std::exception& e)
      {
        MLOG_RED(el::Level::Warning, "error deserialize: " << e.what());
        return;
      }
    }
  }

  void server::sms_put(const std::string& to, const std::string& sms, zyre::resp_t r)
  {
    r(me_, _sms_put(to, sms));
  }

  uint64_t server::_sms_put(const std::string& to, const std::string& sms)
  {
    if(w_ptr_->watch_only())
      return uint64_t(-1);

    std::string from = w_ptr_->get_address_as_str();
    std::string tmp = from + salt_;
    hash_t h_from;
    crypto::cn_fast_hash(tmp.data(), tmp.size(), h_from.u.h);

    tmp = to + salt_;
    hash_t h_to;
    crypto::cn_fast_hash(tmp.data(), tmp.size(), h_to.u.h);

    std::stringstream ss;
    data_t info{ sms, "", to, _sms_label(), from, time(nullptr) };
    for(const auto& a : w_ptr_->get_address_book())
      if(to == cryptonote::get_account_address_as_str(w_ptr_->nettype(), a.m_is_subaddress, a.m_address))
        { info.t = a.m_label; break; }
    msgpack::pack(ss, info);
    zyre::wallet::data_cipher_t cipher;
    try
    {
      zyre::wallet::encrypt(pub_, ss.str(), cipher.d, cipher.u.k, cipher.iv);
    }
    catch (const std::exception& e)
    {
      MLOG_RED(el::Level::Warning, "failed encript message for store, " << e.what());
      return uint64_t(-1);
    }

    uint64_t n = 0;
    data_[CONTACT][h_to][MESSAGES_COUNTER] >> n; n++;
    data_[MESSAGE][h_to][n][h_from][MESSAGE_TOSEND] = cipher;
    data_[CONTACT][h_to][MESSAGES_COUNTER] = n;
    data_[CONTACT][h_to][MESSAGES_TIME] = time(nullptr);

    return n;
  }

  void server::on_received(const std::string& data)
  {
    if(!data_.opened() && !data_.open(path_))
    {
      MLOG_RED(el::Level::Warning, "Error: database is not opened");
      return;
    }
    
    data_version_t  ver;
    data_cipher_t   cipher;

    try
    {
      size_t off = 0;
      msgpack::object_handle v0 = msgpack::unpack(data.data(), data.size(), off);
      v0.get().convert(ver);
      
      if(ver.major != 0 || ver.minor != 1)
      {
        MLOG_RED(el::Level::Warning, "error version of message, ver: " << ver.major << "." << ver.minor);
        return;
      }

      msgpack::object_handle o1 = msgpack::unpack(data.data(), data.size(), off);
      o1.get().convert(cipher);
    }
    catch (const std::exception& e)
    {
      MLOG_RED(el::Level::Warning, "error deserialize: " << e.what());
      return;
    }
    
    crypto::secret_key skey = w_ptr_->get_account().get_keys().m_view_secret_key;
    
    std::string m;
    try
    {
      decrypt(cipher.d, cipher.u.k, cipher.iv, skey, m);
    }
    catch (const std::exception& e)
    {
      MLOG_RED(el::Level::Warning, "error decrypt: " << e.what());
      return;
    }
    
    data_sign_t sign;

    try
    {
      size_t off = 0;
      msgpack::object_handle o1 = msgpack::unpack(m.data(), m.size(), off);
      o1.get().convert(sign);
    }
    catch (const std::exception& e)
    {
      MLOG_RED(el::Level::Warning, "error deserialize: " << e.what());
      return;
    }
    
    cryptonote::address_parse_info info;
    if (!cryptonote::get_account_address_from_str_or_url(info, w_ptr_->nettype(), sign.a))
    {
      MLOG_RED(el::Level::Warning, "failed to parse address");
      return;
    }

    crypto::hash hash;
    std::string tmp(sign.d + sign.n + sign.a);
    crypto::cn_fast_hash(tmp.data(), tmp.size(), hash);

    if (!crypto::check_signature(hash, info.address.m_view_public_key, sign.s.data))
    {
      MLOG_RED(el::Level::Warning, "failed address of signed");
      return;
    }
    
    std::string s_to = w_ptr_->get_subaddress_label({0,0});
    std::string s_to_a = w_ptr_->get_address_as_str();
    
    tmp = s_to_a + salt_;
    hash_t h_to;
    crypto::cn_fast_hash(tmp.data(), tmp.size(), h_to.u.h);

    tmp = sign.a + salt_;
    hash_t h_from;
    crypto::cn_fast_hash(tmp.data(), tmp.size(), h_from.u.h);

    auto address_book = w_ptr_->get_address_book();
    bool a_exist = false;
    
    for (size_t i = 0; i < address_book.size(); ++i)
    {
      auto& row = address_book[i];
      if(row.m_address == info.address)
      {
        std::string s_from = row.m_label;
        //if(s_from != sign.n)
        //{
          //MLOG_RED(el::Level::Warning, "failed address of signed");
          //return;
        //}
        time_t tm = time(nullptr);
        data_t m_info{ sign.d, s_to, s_to_a, sign.n, sign.a, tm };
        std::stringstream ss;
        msgpack::pack(ss, m_info);
        data_cipher_t m_cipher;
        try
        {
          encrypt(pub_, ss.str(), m_cipher.d, m_cipher.u.k, m_cipher.iv);
        }
        catch (const std::exception& e)
        {
          MLOG_RED(el::Level::Warning, "failed encript message for store");
          return;
        }
        uint64_t n = 0;
        if(!w_ptr_->watch_only())
        {
          data_[CONTACT][h_from][MESSAGES_COUNTER] >> n; n++;
          data_[MESSAGE][h_from][n][h_to][MESSAGE_RECEIVED] = m_cipher;
          data_[CONTACT][h_from][MESSAGES_COUNTER] = n;
          data_[CONTACT][h_from][MESSAGES_TIME] = tm;
        }
        else
        {
          data_[CONTACT][h_to][MESSAGES_COUNTER] >> n; n++;
          data_[MESSAGE][h_to][n][h_from][MESSAGE_RECEIVED] = m_cipher;
          data_[CONTACT][h_to][MESSAGES_COUNTER] = n;
          data_[CONTACT][h_to][MESSAGES_TIME] = tm;
        }
        MLOG_RED(el::Level::Warning, "receive message from: " << row.m_description);
        a_exist = true;

        zyre_.call(SHOUT, zyre_.groups(), "sms_put", me_, n, m_info);

        if(on_sms_receive_)
          on_sms_receive_(m_info.fa, m_info.f, m_info.ta, m_info.t, n, m_info.d);

        break;
      }
    }

    if (!a_exist)
    {
      time_t tm = time(nullptr);
      data_t m_info{ sign.d, s_to, s_to_a, sign.n, sign.a, tm };
      std::stringstream ss;
      msgpack::pack(ss, m_info);
      data_cipher_t m_cipher;
      try
      {
        encrypt(pub_, ss.str(), m_cipher.d, m_cipher.u.k, m_cipher.iv);
      }
      catch (const std::exception& e)
      {
        MLOG_RED(el::Level::Warning, "failed encript message for store: " << e.what());
        return;
      }
      uint64_t n = 0;
      if(!w_ptr_->watch_only())
      {
        data_[CONTACT][h_from][MESSAGES_COUNTER] >> n; n++;
        data_[MESSAGE][h_from][n][h_to][MESSAGE_RECEIVED] = m_cipher;
        data_[CONTACT][h_from][MESSAGES_COUNTER] = n;
        data_[CONTACT][h_from][MESSAGES_TIME] = tm;
      }
      else
      {
        data_[CONTACT][h_to][MESSAGES_COUNTER] >> n; n++;
        data_[MESSAGE][h_to][n][h_from][MESSAGE_RECEIVED] = m_cipher;
        data_[CONTACT][h_to][MESSAGES_COUNTER] = n;
        data_[CONTACT][h_to][MESSAGES_TIME] = tm;
      }
      w_ptr_->add_address_book_row(info.address, sign.n, nullptr, std::string(), info.is_subaddress);
      contact_t ci{sign.a, sign.n};
      zyre_.call(SHOUT, zyre_.groups(), "sms_put", me_, n, m_info);
      if(on_sms_receive_)
        on_sms_receive_(m_info.fa, m_info.f, m_info.ta, m_info.t, n, m_info.d);
      MLOG_RED(el::Level::Warning, "receive message from new address: " << sign.n);
    }
  }
 
  bool server::send_to(const data_t& msg)
  {
    auto address_book = w_ptr_->get_address_book();
    for (size_t i = 0; i < address_book.size(); ++i)
    {
      std::string s_to, s_to_a;
      auto& row = address_book[i];

      /*if(row.m_label.empty())
      {
        MLOG_RED(el::Level::Warning, "failed get label address");
        return false;
      }*/

      s_to = row.m_label;
      s_to_a = cryptonote::get_account_address_as_str(w_ptr_->nettype(), row.m_is_subaddress, row.m_address);
      
      if(/*msg.t != s_to ||*/ msg.ta != s_to_a)
        continue;

      data_sign_t sign;
      sign.d = msg.d;
      sign.a = w_ptr_->get_subaddress_as_str({0,0});
      sign.n = w_ptr_->get_subaddress_label({0,0});

      crypto::hash hash;
      std::string tmp(sign.d + sign.n + sign.a);
      crypto::cn_fast_hash(tmp.data(), tmp.size(), hash);
     
      crypto::generate_signature(hash, w_ptr_->get_account().get_keys().m_account_address.m_view_public_key, w_ptr_->get_account().get_keys().m_view_secret_key, sign.s.data); 
      
      std::stringstream ss1;
      msgpack::pack(ss1, sign);
      data_cipher_t m_cipher;

      try
      {
        encrypt(row.m_address.m_view_public_key, ss1.str(), m_cipher.d, m_cipher.u.k, m_cipher.iv);
      }
      catch (const std::exception& e)
      {
        MLOG_RED(el::Level::Warning, "failed encript message for store: " << e.what());
        return false;
      }

      data_version_t ver{0,1};
      std::stringstream ss2;
      msgpack::pack(ss2, ver);
      msgpack::pack(ss2, m_cipher);

      std::vector<uint8_t> extra;
      cryptonote::tx_extra_sms em{SMS_LAYER, ss2.str()};
      cryptonote::set_sms_to_extra(extra, em);

      cryptonote::tx_destination_entry de;
      de.amount = CRYPTONOTE_MESSAGE_TX_AMOUNT;
      de.addr = row.m_address;
      de.is_integrated = false;
      de.is_subaddress = false;

      std::vector<cryptonote::tx_destination_entry> dsts;
      dsts.push_back(de);

      try
      {
        std::vector<tools::wallet2::pending_tx> ptx_vector =
          w_ptr_->create_transactions_2(
            dsts,
            0 /* fake_outs_count */,
            0 /* unlock_time */,
            0 /* priority */,
            extra,
            0 /* m_current_subaddress_account */,
            {}
          );

        for(auto& ptx : ptx_vector)
        {
          w_ptr_->commit_tx(ptx);
        }
      }
      catch(const std::exception& e)
      {
        MWARNING("error create tx: " << e.what());
        return false;
      }

      break;
    }
    return true;
  }
  
  void server::send()
  {
    if(!data_.opened() && !data_.open(path_))
    {
      MLOG_RED(el::Level::Warning, "Error: database is not opened");
      return;
    }

    std::string my_id = w_ptr_->get_address_as_str();
    std::string tmp = my_id + salt_;
    crypto::hash h_my;
    crypto::cn_fast_hash(tmp.data(), tmp.size(), h_my);

    lldb::cxx::db mm = data_[MESSAGE];
    for (auto m = mm.begin(); m != mm.end(); m++)
    {
      hash_t h_to; uint64_t n; hash_t h_from; uint64_t f;
      m->first >> h_to >> n >> h_from >> f;

      //MLOG_RED(el::Level::Warning, "batch h_from: "
        //<< epee::string_tools::pod_to_hex(h_from.u.h)
        //<< "\n\t h_my: "
        //<< epee::string_tools::pod_to_hex(h_my));
      
      if((f & MESSAGE_TOSEND) == 0)
        continue;
      
      if(0 != memcmp(h_from.u.h.data, h_my.data, sizeof(h_my.data)))
        continue;

      uint64_t n_sended = 0;
      if((data_[CONTACT][h_to][MESSAGES_SENDED] >> n_sended) && n <= n_sended)
        continue;

      data_cipher_t m_cipher;
      m->second >> m_cipher;
      
      std::string tmp;
      try
      {
        decrypt(m_cipher.d, m_cipher.u.k, m_cipher.iv, sec_, tmp);
      }
      catch (const std::exception& e)
      {
        MLOG_RED(el::Level::Warning, "failed decript message for send: " << e.what());
        return;
      }
      
      data_t msg;
      msgpack::object_handle v = msgpack::unpack(tmp.data(), tmp.size());
      v.get().convert(msg);
      
      if(send_to(msg))
        data_[CONTACT][h_to][MESSAGES_SENDED] = n;
    }
  }

  void server::set_smskey(const crypto::secret_key& sec)
  {
    sec_ = sec;
    pub_ = get_sms_public_key(sec);
    salt_ = get_sms_salt(sec);
  }

  void server::on_new_block(uint64_t height, const cryptonote::block& block) { if(callback_) callback_->on_new_block(height, block); }
  void server::on_money_received(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& tx, uint64_t amount, const cryptonote::subaddress_index& subaddr_index, bool is_change, uint64_t unlock_time) { if(callback_) callback_->on_money_received(height, txid, tx, amount, subaddr_index, is_change, unlock_time); }
  void server::on_sms_received(uint64_t /*height*/, const crypto::hash &/*txid*/, const cryptonote::transaction& /*tx*/, uint32_t layer, const std::string& data, uint64_t amount, const cryptonote::subaddress_index& /*subaddr_index*/) { if(layer == SMS_LAYER && amount >= CRYPTONOTE_MESSAGE_TX_AMOUNT) on_received(data); }
  void server::on_unconfirmed_money_received(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& tx, uint64_t amount, const cryptonote::subaddress_index& subaddr_index) { if(callback_) callback_->on_unconfirmed_money_received(height, txid, tx, amount, subaddr_index); }
  void server::on_money_spent(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& in_tx, uint64_t amount, const cryptonote::transaction& spend_tx, const cryptonote::subaddress_index& subaddr_index) { if(callback_) callback_->on_money_spent(height, txid, in_tx, amount, spend_tx, subaddr_index); }
  void server::on_skip_transaction(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& tx) { if(callback_) callback_->on_skip_transaction(height, txid, tx); }
  boost::optional<epee::wipeable_string> server::on_get_password(const char *reason) {  if(callback_) callback_->on_get_password(reason); return boost::none; }
  void server::on_lw_new_block(uint64_t height) { if(callback_) callback_->on_lw_new_block(height); }
  void server::on_lw_money_received(uint64_t height, const crypto::hash &txid, uint64_t amount) { if(callback_) callback_->on_lw_money_received(height, txid, amount); }
  void server::on_lw_unconfirmed_money_received(uint64_t height, const crypto::hash &txid, uint64_t amount) { if(callback_) callback_->on_lw_unconfirmed_money_received(height, txid, amount); }
  void server::on_lw_money_spent(uint64_t height, const crypto::hash &txid, uint64_t amount) { if(callback_) callback_->on_lw_money_spent(height, txid, amount); }
  void server::on_device_button_request(uint64_t code) { if(callback_) callback_->on_device_button_request(code); }
  void server::on_device_button_pressed() { if(callback_) callback_->on_device_button_pressed(); }
  boost::optional<epee::wipeable_string> server::on_device_pin_request() {  if(callback_) return callback_->on_device_pin_request(); return boost::none; }
  boost::optional<epee::wipeable_string> server::on_device_passphrase_request(bool & on_device) { if(callback_) return callback_->on_device_passphrase_request(on_device); on_device = true; return boost::none; }
  void server::on_device_progress(const hw::device_progress& event) { if(callback_) callback_->on_device_progress(event); };
  void server::on_pool_tx_removed(const crypto::hash &txid) { if(callback_) callback_->on_pool_tx_removed(txid); }

}}
