#ifndef __sms_h__
#define __sms_h__

#include <boost/thread/mutex.hpp>
#include <boost/asio/io_service.hpp>

#include "wallet/wallet_zyre.h"

#define GYRO_SMS "gyro-sms"

class sms
{
  boost::asio::io_service& ios_;
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

  void put_label(const std::string& src, const std::string& lebel);
  void put_addr_book(const std::string& src, const zyre::wallet::contacts_t& a);
  void put_count(const std::string& src, const uint64_t& c);
  void put_addr(const std::string& src, const zyre::wallet::contact_t& c);
  void del_addr(const std::string& src, const std::string& addr);
  void put_1(const std::string& src, uint64_t n, const zyre::wallet::data_t& msg);
  void put_array(const std::string& src, const std::vector<std::pair<uint64_t, zyre::wallet::data_t>>& array);

  virtual void contact_add(const std::string& id, const std::string& name, const std::string& key, const std::string& src) = 0;
  virtual void contact_remove(const std::string& id) = 0;
  virtual void message_received_from(const std::string& to, int n, const std::string& from) = 0;

  zyre::client& zyre() { return zyre_; }
};

#endif
