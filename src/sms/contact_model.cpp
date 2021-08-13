#include <cstdlib>
#include <ctime>

#include "contact_model.h"

#include <QCoreApplication>
#include <QDateTime>

#include "misc_log_ex.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "wallet.sms"

contact_model::contact_model()
  : QAbstractListModel(nullptr)
{
}

contact_model::~contact_model()
{
}

int contact_model::rowCount(const QModelIndex &) const
{
    return int(data_.size());
}

QHash<int, QByteArray> contact_model::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[ID] = "id";
  roles[NAME] = "name";
  roles[ME] = "me";
  roles[SHORT_NAME] = "short_name";
  roles[SHORT_COLOR] = "short_color";
  roles[SHORT_BACKGROUND] = "short_background";
  roles[UNREAD_CNT] = "unread_cnt";
  roles[TM] = "tm";
  roles[TM_SORT] = "tm_sort";
  return roles;
}

QVariant contact_model::data(const QModelIndex &index, int role) const
{
  if(index.isValid() && index.row() < int(data_.size()))
  {
    const _info_& info = data_[size_t(index.row())];

    uint64_t tm = time(NULL);
    if(role == TM || role == TM_SORT)
    {
      auto d = db_;
      d[CONTACT][info.h_id][MESSAGES_TIME] >> tm;
    }

    uint64_t c = 0, v = 0;
    if(role == UNREAD_CNT)
    {
      auto d = db_;
      d[CONTACT][info.h_id][MESSAGES_COUNTER] >> c;
      d[CONTACT][info.h_id][MESSAGES_VIEWED] >> v;
    }

    switch (role)
    {
      case ID: return info.id.c_str();
      case NAME: return info.name.c_str();
      case ME: return info.me.c_str();
      case SHORT_NAME: return info.id.substr(0,2).c_str();
      case SHORT_COLOR: return "#303030";
      case SHORT_BACKGROUND: return "#d0d0d0";
      case TM: return QDateTime().fromSecsSinceEpoch(tm).toString("dd.MM.yyyy hh:mm");
      case TM_SORT: return QDateTime().fromSecsSinceEpoch(tm).toString("dd.MM.yyyy hh");
      case UNREAD_CNT: return int(c-v);
    }
  }

  return QVariant();
}

void contact_model::add(std::string id, std::string name, std::string src, bool isgroup)
{
  for(auto c=data_.begin(); c<data_.end(); c++)
    if(c->id == id)
    {
      MLOG_RED(el::Level::Warning, "add already exist: " << id << " name: " << name);
      return;
    }

  beginInsertRows(QModelIndex(), int(data_.size()), int(data_.size()));
  data_.push_back({id, name, src, isgroup});
  endInsertRows();
}

void contact_model::remove(std::string id)
{
  int n = 0;
  for(auto c=data_.begin(); c<data_.end(); c++, n++)
    if(c->id == id)
    {
      beginRemoveRows(QModelIndex(), n, n);
      data_.erase(c);
      endRemoveRows();
      break;
    }
}

sort_contact_model::sort_contact_model()
    : QSortFilterProxyModel(nullptr)
{
  setSourceModel(&src_);
  setSortRole(contact_model::TM_SORT);
  setFilterRole(contact_model::NAME);
  setSortOrder(true);
}

sort_contact_model::~sort_contact_model()
{
}

void sort_contact_model::setFilterString(QString string)
{
  this->setFilterCaseSensitivity(Qt::CaseInsensitive);
  this->setFilterFixedString(string);
}

void sort_contact_model::setSortOrder(bool checked)
{
  if(checked)
    this->sort(0, Qt::DescendingOrder);
  else
    this->sort(0, Qt::AscendingOrder);
}

contact_model& sort_contact_model::source()
{
  return src_;
}

void sort_contact_model::invalidate_sort()
{
  invalidate();
}
