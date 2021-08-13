#ifndef CONTACT_MODEL_H
#define CONTACT_MODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

#include "zyre/db.hpp"
#include "wallet/wallet_zyre.h"

#include "main.h"

class contact_model : public QAbstractListModel
{
  Q_OBJECT

public:
  enum Fields { ID=0, NAME, ME, SHORT_NAME, SHORT_COLOR, SHORT_BACKGROUND, UNREAD_CNT, TM, TM_SORT };

  contact_model(sms *s, const std::string& path);

  virtual ~contact_model() override;

  Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  Q_INVOKABLE QVariant data(const QModelIndex &index, int role) const override;

  Q_SLOT void add(std::string id, std::string name, std::string key, std::string me);
  Q_SLOT void remove(std::string id);

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  sms                       *s_;

  typedef struct {
    std::string id;     // gyro address as id
    std::string name;   // label as mail e.t.c alex@gyro.bit
    std::string key;    // hex view secret key for group or channel, null for peer to peer
    std::string me;     // gyro address of my source wallet
    zyre::wallet::hash_t h_id;
  } _info_;

  std::vector<_info_>       data_;
  lldb::cxx::db             db_;
};

class sort_contact_model : public QSortFilterProxyModel
{
  Q_OBJECT
  contact_model src_;

public:
  Q_ENUM(contact_model::Fields)

  sort_contact_model(sms *s, const std::string& path);
  ~sort_contact_model();

  Q_INVOKABLE void setFilterString(QString string);
  Q_INVOKABLE void setSortOrder(bool checked);

  Q_INVOKABLE void invalidate_sort();

  contact_model& source();
};

#endif
