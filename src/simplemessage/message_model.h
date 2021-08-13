#ifndef MESSAGES_H
#define MESSAGES_H

//#include <QQmlApplicationEngine>
//#include <QQmlEngine>
//#include <QQmlContext>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>

#include <crypto/crypto.h>

#include "lldb/db.hpp"
#include "wallet/wallet_message.h"

class message_model : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY(QByteArray current_id READ get_current_id WRITE set_current_id NOTIFY current_id_changed)

public:
  message_model(const crypto::secret_key& sec, const std::string& path);
  virtual ~message_model();

  enum message_fields { BODY, TM, FROM, NAME, SHORT_FROM, SHORT_COLOR, SHORT_BACKGROUND, N_INDEX, SENDED };

  Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
  Q_INVOKABLE QVariant data(const QModelIndex &index, int role) const;
  Q_INVOKABLE void append(const QString& message);
  Q_INVOKABLE void set_current_contact(const QByteArray& id, const QByteArray& name);

  Q_SLOT void on_received_from(std::string to, int n_index, std::string from);
  Q_SLOT void on_changed(std::string to, int n_index);

  QByteArray get_current_id() const;
  void set_current_id(const QByteArray& id);

  void set_self_id(const std::string& id) { self_id_ = id; }
  void set_self_name(const std::string& name) { self_name_ = name; }

signals:
  void current_id_changed();
  void contacts_invalidate();
  void do_send_to(std::string to, int n_index);
  void on_id_change(std::string id) const;

protected:
  QHash<int, QByteArray> roleNames() const;

private:
  gyro::leveldb::cxx::db   data_;
  std::string     salt_;
  crypto::public_key pub_;
  crypto::secret_key sec_;
  std::string     current_id_;
  std::string     current_name_;
  std::string     self_id_;
  std::string     self_name_;
};

class sort_message_model : public QSortFilterProxyModel
{
  Q_OBJECT
  Q_PROPERTY(QByteArray current_id READ get_current_id WRITE set_current_id NOTIFY current_id_changed)

  message_model src_;
public:
  Q_ENUM(message_model::message_fields)

  sort_message_model(const crypto::secret_key& sec, const std::string& data_dir);
  ~sort_message_model();

  Q_INVOKABLE void setFilterString(QString string);
  Q_INVOKABLE void setSortOrder(bool checked);

  Q_INVOKABLE void append(const QString& message);
  Q_INVOKABLE void set_current_contact(const QByteArray& id, const QByteArray& name);

  QByteArray get_current_id() const;
  void set_current_id(const QByteArray& id);

  message_model& source();

signals:
  void current_id_changed();
};

#endif
