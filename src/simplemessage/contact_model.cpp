#include <cstdlib>
#include <ctime>

#include "contact_model.h"

#include <QCoreApplication>
#include <QDateTime>

#include "misc_log_ex.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "wallet.simplemessage"

contact_model::contact_model(const std::vector<std::pair<std::string, std::string>>& data, const crypto::secret_key& sec, const std::string& path)
    : QAbstractListModel(nullptr)
    , sec_(sec)
    , db_(path)
{
  crypto::secret_key_to_public_key(sec_, pub_);
  salt_ = gyro::sms::sync::get_salt(sec_);
  for(auto c : data)
    data_.push_back({c.first, c.second});
}

contact_model::~contact_model()
{
}

QByteArray contact_model::self_id()
{
    return self_id_.c_str();
}

QByteArray contact_model::self_name()
{
    return self_name_.c_str();
}

int contact_model::rowCount(const QModelIndex &) const
{
    return int(data_.size());
}

QHash<int, QByteArray> contact_model::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ID] = "id";
    roles[NAME] = "name";
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
          std::string tmp = info.id + salt_;
          gyro::sms::hash_t h_id;
          crypto::cn_fast_hash(tmp.data(), tmp.size(), h_id.u.h);
          auto d = db_;
          d[CONTACT][h_id.u.r][MESSAGES_TIME] >> tm;
        }

        switch (role)
        {
            case ID: return info.id.c_str();
            case NAME: return info.name.c_str();
            case SHORT_NAME: return info.id.substr(0,2).c_str();
            case SHORT_COLOR: return "#303030";
            case SHORT_BACKGROUND: return "#d0d0d0";
            case TM: return QDateTime().fromSecsSinceEpoch(tm).toString("dd.MM.yyyy hh:mm");
            case TM_SORT: return QDateTime().fromSecsSinceEpoch(tm).toString("dd.MM.yyyy hh");
            case UNREAD_CNT: return int(0);
        }
    }

    return QVariant();
}

void contact_model::add(std::string id, std::string name)
{
  for(auto c=data_.begin(); c<data_.end(); c++)
    if(c->id == id)
    {
      MLOG_RED(el::Level::Warning, "add already exist: " << id << " name: " << name);
      return;
    }

  beginInsertRows(QModelIndex(), int(data_.size()), int(data_.size()));
  data_.push_back({id, name});
  //std::sort(data_.begin(), data_.end(),
    //[](const _info_& i1, const _info_& i2){return i1.tm > i2.tm; });
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

sort_contact_model::sort_contact_model(const std::vector<std::pair<std::string, std::string>>& data, const crypto::secret_key& sec, const std::string& path)
    : QSortFilterProxyModel(nullptr)
    , src_(data, sec, path)
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
    {
        this->sort(0, Qt::DescendingOrder);
    }
    else
    {
        this->sort(0, Qt::AscendingOrder);
    }
}

QByteArray sort_contact_model::self_id()
{
    return src_.self_id();
}

QByteArray sort_contact_model::self_name()
{
    return src_.self_name();
}

contact_model& sort_contact_model::source()
{
    return src_;
}

void sort_contact_model::invalidate_sort()
{
    invalidate();
}
