#ifndef __console_h__
#define __console_h__

#include <QObject>
#include <boost/thread/mutex.hpp>
#include <boost/asio/io_service.hpp>

#include "wallet/wallet_zyre.h"

#define GYRO_SMS "gyro-sms"

class sms : public QObject
{
  Q_OBJECT

  friend class contact_model;
  friend class message_model;

  boost::asio::io_service ios_;
protected:
  boost::mutex m_me_mutex_;
  typedef struct {
    std::string         id;
    std::string         name;
    std::string         salt;
    crypto::public_key  pub;
    crypto::secret_key  sec;
  } me_info_t;
  std::map<std::string, me_info_t> me_;
  lldb::cxx::db         data_;
  zyre::server          zsrv_;
  zyre::client          zyre_;
  std::thread           thread_;
  std::vector<std::string> groups_;

public:
  sms(const std::vector<std::string>& group, const std::string& path, const std::string& pin);
  virtual ~sms();

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

  Q_SIGNAL void contact_add(std::string id, std::string name, std::string key, std::string me);
  Q_SIGNAL void contact_remove(std::string id);

  Q_SIGNAL void message_received_from(std::string to, int n, std::string from);
  //Q_SIGNAL void message_changed(std::string to, int n);

  //Q_SLOT void do_send_to(std::string to, int n, std::string me);

  zyre::client& zyre() { return zyre_; }
};

#endif
