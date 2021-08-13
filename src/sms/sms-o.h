#ifndef __sms_h__
#define __sms_h__

#include <boost/thread/mutex.hpp>
#include <boost/asio/io_service.hpp>

#include "wallet/wallet_zyre.h"

#define GYRO_SMS "gyro-sms"

class sms
{
  boost::asio::io_service& ios_;

public:
  typedef struct {
    std::string         id;
    std::string         name;
    std::string         salt;
    crypto::public_key  pub;
    crypto::secret_key  sec;
  } me_info_t;

  boost::mutex m_me_mutex_;
  std::map<std::string, me_info_t> me_;

protected:
  lldb::cxx::db         data_;
  zyre::server          zsrv_;
  zyre::client          zyre_;
  std::thread           thread_;
  std::vector<std::string> groups_;

public:
  sms(boost::asio::io_service& ios, const std::vector<std::string>& group, const std::string& path, const std::string& pin);

  void join(const std::string& name);
  void leave(const std::string& name);

  void start();
  void stop();

  void put_wallet(const std::string& id, const std::string& name, const zyre::wallet::sec_t& sec);
  void put_contacts(const std::string& me, const zyre::wallet::contacts_t& a);
  void put_contact(const std::string& me, const zyre::wallet::contact_t& c);
  void del_contact(const std::string& me, const std::string& id);
  void put_message(const std::string& me, const uint64_t& n, const zyre::wallet::data_cipher_t& cipher);
  void put_messages(const std::string& me, const std::vector<std::pair<uint64_t, zyre::wallet::data_cipher_t>>& mm);

  virtual void contact_add(const std::string& id, const std::string& name, const std::string& key, const std::string& me) = 0;
  virtual void contact_remove(const std::string& id) = 0;
  virtual void message_received_from(const std::string& to, int n, const std::string& from) = 0;

  zyre::client& zyre() { return zyre_; }
};

#endif
