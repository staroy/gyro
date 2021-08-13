#ifndef CONTACT_MODEL_H
#define CONTACT_MODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

#include "simplemessage.h"

#include "lldb/db.hpp"
#include "wallet/wallet_message.h"

class contact_model : public QAbstractListModel
{
  Q_OBJECT

public:
  enum Fields { ID=0, NAME, SHORT_NAME, SHORT_COLOR, SHORT_BACKGROUND, UNREAD_CNT, TM, TM_SORT };

  contact_model(const std::vector<std::pair<std::string, std::string>>& data, const crypto::secret_key& sec, const std::string& path);

  virtual ~contact_model() override;

  Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  Q_INVOKABLE QVariant data(const QModelIndex &index, int role) const override;

  Q_INVOKABLE QByteArray self_id();
  Q_INVOKABLE QByteArray self_name();
  
  Q_SLOT void add(std::string id, std::string name);
  Q_SLOT void remove(std::string id);

  //bool event(QEvent *e) override;

  void set_self_id(const std::string& id) { self_id_ = id; }
  void set_self_name(const std::string& name) { self_name_ = name; }

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  std::string         self_id_;
  std::string         self_name_;

  typedef struct {
    std::string id;
    std::string name;
  } _info_;

  std::vector<_info_> data_;
  gyro::leveldb::cxx::db  db_;
  std::string     salt_;
  crypto::public_key pub_;
  crypto::secret_key sec_;
};

class sort_contact_model : public QSortFilterProxyModel
{
  Q_OBJECT
  contact_model src_;

public:
  Q_ENUM(contact_model::Fields)

  sort_contact_model(const std::vector<std::pair<std::string, std::string>>& data, const crypto::secret_key& sec, const std::string& path);
  ~sort_contact_model();

  Q_INVOKABLE void setFilterString(QString string);
  Q_INVOKABLE void setSortOrder(bool checked);

  Q_INVOKABLE void invalidate_sort();

  Q_INVOKABLE QByteArray self_id();
  Q_INVOKABLE QByteArray self_name();

  contact_model& source();
};

#endif
