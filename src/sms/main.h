#ifndef __console_h__
#define __console_h__

#include <QObject>
#include "sms.h"

class sms2 : public QObject, public sms
{
  Q_OBJECT

public:
  sms2(boost::asio::io_service& ios, const std::vector<std::string>& group, const std::string& path, const std::string& pin);

  void contact_add(const std::string& id, const std::string& name, const std::string& key, const std::string& me);
  void contact_remove(const std::string& id);
  void message_received_from(const std::string& to, int n, const std::string& from);

  Q_SIGNAL void contact_add_2(std::string id, std::string name, std::string key, std::string me);
  Q_SIGNAL void contact_remove_2(std::string id);
  Q_SIGNAL void message_received_from_2(std::string to, int n, std::string from);
};

#endif
