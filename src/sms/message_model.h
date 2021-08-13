#ifndef MESSAGE_MODEL_H
#define MESSAGE_MODEL_H

//#include <QQmlApplicationEngine>
//#include <QQmlEngine>
//#include <QQmlContext>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>

#include <crypto/crypto.h>

#include "zyre/db.hpp"
#include "wallet/wallet_zyre.h"

#include "main.h"

class message_model : public QAbstractListModel
{
  Q_OBJECT

  Q_PROPERTY(QByteArray contact READ get_contact WRITE set_contact NOTIFY contact_changed)
  Q_PROPERTY(QByteArray contact_id READ get_contact_id)
  Q_PROPERTY(QByteArray contact_name READ get_contact_name)

public:
  message_model();
  virtual ~message_model();

  enum message_fields { BODY, TM, FROM, NAME, SHORT_FROM, SHORT_COLOR, SHORT_BACKGROUND, N_INDEX, SENT_BY_ME, SENT_BY_CURRENT, SENDED };

  Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
  Q_INVOKABLE QVariant data(const QModelIndex &index, int role) const;
  Q_INVOKABLE void append(const QString& message);

  Q_SLOT void on_received_from(
    std::string from,
    std::string from_name,
    int n,
    std::string to,
    std::string to_name,
    std::string txt,
    time_t ts,
    std::string src);

  Q_SLOT void on_set_count(std::string to, int n);
  Q_SLOT void on_set_viewed(std::string to, int n);

  void set_contact(const QByteArray&);
  QByteArray get_contact() const;
  QByteArray get_contact_id() const;
  QByteArray get_contact_name() const;

signals:
  void contact_changed();
  void contacts_invalidate();

protected:
  QHash<int, QByteArray> roleNames() const;

private:
  std::string               id_;
  std::string               name_;
  std::string               src_;
  std::map<std::string, int> count_;
  std::map<std::string, int> viewed_;
  std::map<std::string, std::map<int, zyre::wallet::data_t>> data_;
};

class sort_message_model : public QSortFilterProxyModel
{
  Q_OBJECT

  Q_PROPERTY(QByteArray contact READ get_contact WRITE set_contact)
  Q_PROPERTY(QByteArray contact_id READ get_contact_id)
  Q_PROPERTY(QByteArray contact_name READ get_contact_name)

  message_model src_;
public:
  Q_ENUM(message_model::message_fields)

  sort_message_model(sms *s, const std::string& path);
  ~sort_message_model();

  Q_INVOKABLE void setFilterString(QString string);
  Q_INVOKABLE void setSortOrder(bool checked);

  Q_INVOKABLE void append(const QString& message);

  void set_contact(const QByteArray& c);
  QByteArray get_contact() const;
  QByteArray get_contact_id() const;
  QByteArray get_contact_name() const;

  message_model& source();

signals:
  void current_id_changed();
};

#endif