#ifndef __wallet_zyre_h__
#define __wallet_zyre_h__

#include <string>
#include <atomic>
#include <thread>

#include <boost/thread/lock_guard.hpp>

#include "cryptonote_basic/cryptonote_boost_serialization.h"
#include "cryptonote_basic/subaddress_index.h"

#include "crypto/crypto.h"
#include "wallet2.h"

#include "zyre/db.hpp"
#include "zyre/zyre.hpp"
#include "wallet_zyre_rpc_proxy.h"
#include "wallet_zyre_util.h"

#define CONTACT 1 // prefix for contacts info
#define MESSAGE 2 // prefix for heap messages

#define MESSAGE_RECEIVED 1 // postfix for received messages type 
#define MESSAGE_TOSEND 2 // postfix for messages type that to send

#define MESSAGES_COUNTER 1 // postfix for contact count messages
#define MESSAGES_TIME 2 // postfix for contact last time of messages
#define MESSAGES_SENDED 3
#define MESSAGES_VIEWED 4 // postfix for contact viewed messages

namespace zyre { namespace wallet {

  typedef struct {
    union {
      crypto::hash h;
      unsigned char r[32];
    } u;
    MSGPACK_DEFINE(u.r)
  } hash_t;
  
  struct data_t {
    std::string d, // message body
                t, ta, // to label e.t.c alex@gyro.bit, address
                f, fa; // from label, address
    time_t ts; // timestamp
    MSGPACK_DEFINE_MAP(d, t, ta, f, fa, ts)
  };

  struct data_cipher_t
  {
    std::string d;
    crypto::chacha_iv iv;
    union {
      crypto::public_key k;
      unsigned char r[32];
    } u;
    MSGPACK_DEFINE_MAP(d, iv.data, u.r)
  };

  struct sec_t
  {
    union {
      crypto::ec_scalar k;
      unsigned char r[32];
    } u;
    MSGPACK_DEFINE(u.r)
  };

  typedef struct
  {
    std::string address;
    std::string name;
    MSGPACK_DEFINE(address, name)
  } contact_t;

  typedef std::vector<contact_t> contacts_t;
  typedef std::function<
    void(const std::string&,    /* from address */
         const std::string&,    /* from label */
         const std::string&,    /* to address */
         const std::string&,    /* to label */
         uint64_t,              /* n index */
         const std::string&)    /* sms text */
         > on_sms_receive_callback_t;

  class server : public tools::i_wallet2_callback
  {
    boost::asio::io_service ios_;
    tools::wallet2 *w_ptr_;
    lldb::cxx::db data_;
    std::string path_;
    tools::i_wallet2_callback* callback_;
    zyre::server zsrv_;
    zyre::client zyre_;
    std::string me_;
    std::string salt_;
    crypto::secret_key sec_;
    crypto::public_key pub_;
    rpc_proxy rpc_;
    std::map<std::string, func_r_t> meth_;
    std::thread thr_;
    on_sms_receive_callback_t on_sms_receive_;

    bool send_to(const data_t& msg);
    void on_received(const std::string& data);

  public:
    server(tools::wallet2 *w_ptr, const std::string& path);

    zyre::client& zyre() { return zyre_; }

    void set_smskey(const crypto::secret_key& sec);

    void start();
    void stop();

    void send();

    std::string _sms_label();
    uint64_t _sms_count(const std::string& from);
    void _sms_array(
            const std::string& from,
            const uint64_t& n,
            const uint64_t& c,
            std::vector<std::string>& sms,
            std::vector<bool>& sended,
            std::vector<uint64_t>& ts);
    void _sms_array(
            const uint64_t& n,
            const uint64_t& c,
            std::vector<std::string>& from,
            std::vector<std::string>& sms,
            std::vector<uint64_t>& ts);
    void _sms_get(const std::string& from, const uint64_t& n, std::string& sms, bool& sended, uint64_t& ts);
    void _sms_get(const uint64_t& n, std::string& from, std::string& sms, uint64_t& ts);
    uint64_t _sms_put(const std::string& to, const std::string& sms);

    void sms_label(zyre::resp_t r);
    void sms_addr_book(zyre::resp_t r);
    void sms_count(const std::string& from, zyre::resp_t r);
    void sms_array(const std::string& from, const uint64_t& n, const uint64_t& c, zyre::resp_t r);
    void sms_get(const std::string& from, const uint64_t& n, zyre::resp_t r);
    void sms_put(const std::string& to, const std::string& sms, zyre::resp_t r);

    void set_sms_receive_callback(const on_sms_receive_callback_t& f) { on_sms_receive_ = f; }

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

  };

}}

#endif
