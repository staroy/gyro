#include "message_model.h"

#include <cstdlib>
#include <ctime>

#include <QDateTime>

#include "misc_log_ex.h"
#include "string_tools.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "wallet.sms"

message_model::message_model(sms *s, const std::string& path)
  : QAbstractListModel(nullptr)
  , s_(s)
  , data_(path)
{
}

message_model::~message_model()
{
}

int message_model::rowCount(const QModelIndex &) const
{
  auto data = data_;
  std::string tmp = id_ + me_info_.salt;
  zyre::wallet::hash_t h_current;
  crypto::cn_fast_hash(tmp.data(), tmp.size(), h_current.u.h);
  uint64_t n;
  if(!(data[CONTACT][h_current][MESSAGES_COUNTER] >> n))
    return 0;
  return int(n);
}

QHash<int, QByteArray> message_model::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[BODY] = "body";
  roles[TM] = "tm";
  roles[FROM] = "from";
  roles[NAME] = "name";
  roles[SHORT_FROM] = "short_from";
  roles[SHORT_COLOR] = "short_color";
  roles[SHORT_BACKGROUND] = "short_background";
  roles[N_INDEX] = "n_index";
  roles[SENT_BY_ME] = "sent_by_me";
  roles[SENT_BY_CURRENT] = "sent_by_current";
  roles[SENDED] = "sended";
  return roles;
}

QVariant message_model::data(const QModelIndex &index, int role) const
{
  static std::unordered_map<uint64_t,
          std::pair<uint64_t, zyre::wallet::data_t>> heap;
  static std::string id;

  if(!index.isValid())
    return QVariant();
  
  if(id != id_)
  {
    heap.clear();
    id = id_;
  }
  
  auto data = data_;

  std::string tmp = id_ + me_info_.salt;
  zyre::wallet::hash_t h_current;
  crypto::cn_fast_hash(tmp.data(), tmp.size(), h_current.u.h);
  
  lldb::cxx::db current_data = data[MESSAGE][h_current];
  
  zyre::wallet::data_t *p_info = nullptr;
  bool sent_by_me = false;

  uint64_t n;
  int row = index.row();

  auto it = current_data.begin() += row;
  if(it != current_data.end())
  {
    zyre::wallet::hash_t h_from; uint64_t rtx;
    it->first >> n >> h_from >> rtx;
    
    if(heap.find(n) == heap.end())
    {
      zyre::wallet::data_cipher_t cipher;
      it->second >> cipher;

      std::string buf;
      try {
        zyre::wallet::decrypt(cipher.d, cipher.u.k, cipher.iv, me_info_.sec, buf);
      } catch (const std::exception& e) {
        MLOG_RED(el::Level::Warning, "error decrypt: " << e.what());
        return QVariant();
      }

      zyre::wallet::data_t info;
      try {
        msgpack::object_handle o1 = msgpack::unpack(buf.data(), buf.size());
        o1.get().convert(info);
      } catch (const std::exception& e) {
        MLOG_RED(el::Level::Warning, "error deserialize: " << e.what());
        return QVariant();
      }

      heap[n] = {rtx, info};
    }
    sent_by_me = heap[n].first & MESSAGE_TOSEND;
    p_info = &heap[n].second;
  }

  if(p_info)
  {
    switch (role)
    {
      case FROM: return p_info->fa.c_str();
      case BODY: return p_info->d.c_str();
      case TM: return QDateTime().fromSecsSinceEpoch(p_info->ts).toString("dd.MM.yyyy hh:mm");
      case NAME: return p_info->f.c_str();
      case SHORT_FROM: return p_info->fa.substr(0,2).c_str();
      case SHORT_COLOR: return "#303030";
      case SHORT_BACKGROUND: return "#d0d0d0";
      case N_INDEX: return qulonglong(n);
      case SENT_BY_ME: return sent_by_me;
      case SENT_BY_CURRENT: return p_info->fa == id;
      case SENDED: return true;
    }
  }

  return QVariant();
}

void message_model::append(const QString& message)
{
  if(id_.size() > 0)
  {
    std::string tmp = id_ + me_info_.salt;
    zyre::wallet::hash_t h_to;
    crypto::cn_fast_hash(tmp.data(), tmp.size(), h_to.u.h);
    
    tmp = me_info_.id + me_info_.salt;
    zyre::wallet::hash_t h_from;
    crypto::cn_fast_hash(tmp.data(), tmp.size(), h_from.u.h);

    time_t tm = time(nullptr);
    
    zyre::wallet::data_t info{ message.toUtf8().constData(), name_, id_, me_info_.name, me_info_.id, tm };
    std::stringstream ss;
    msgpack::pack(ss, info);
    zyre::wallet::data_cipher_t cipher;
    try {
      zyre::wallet::encrypt(me_info_.pub, ss.str(), cipher.d, cipher.u.k, cipher.iv);
    } catch (const std::exception& e) {
      MLOG_RED(el::Level::Warning, "failed encript message for store");
      return;
    }
    uint64_t n = 0;
    data_[CONTACT][h_to][MESSAGES_COUNTER] >> n;
    beginInsertRows(QModelIndex(), int(n), int(n));
    n++;
    data_[MESSAGE][h_to][n][h_from][MESSAGE_TOSEND] = cipher;
    data_[CONTACT][h_to][MESSAGES_COUNTER] = n;
    data_[CONTACT][h_to][MESSAGES_TIME] = tm;
    data_[CONTACT][h_to][MESSAGES_VIEWED] = n;
    endInsertRows();
    //emit do_send_to(id_, int(n), me_info_.id);
    s_->zyre().call(SHOUT, s_->zyre().groups(), "put_message", h_to, n, h_from, cipher);
  }
}

void message_model::on_received_from(std::string to, int n, std::string from)
{
  if((to == me_info_.id && from == id_) || to == id_)
  {
    beginInsertRows(QModelIndex(), n, n);
    endInsertRows();
    emit contacts_invalidate();
  }
}

//void message_model::on_changed(std::string to, int n)
//{
//  if(to == id_)
//    emit dataChanged(index(n, 0), index(n, 0));
//}

void message_model::set_contact(const QByteArray& c)
{
  beginResetModel();
  if(c.length()>0)
  {
    auto a = c.split(':');
    id_ = a[0].constData();
    name_ = a[1].constData();
    boost::unique_lock<boost::mutex> lock(s_->m_me_mutex_);
    me_info_ = s_->me_[a[2].constData()];
    std::string tmp = id_ + me_info_.salt;
    zyre::wallet::hash_t h_id;
    crypto::cn_fast_hash(tmp.data(), tmp.size(), h_id.u.h);
    uint64_t c = 0, v = 0;
    data_[CONTACT][h_id][MESSAGES_COUNTER] >> c;
    data_[CONTACT][h_id][MESSAGES_VIEWED] >> v;
    if(c > v)
    {
      data_[CONTACT][h_id][MESSAGES_VIEWED] = c;
      emit contacts_invalidate();
    }
  }
  else
  {
    id_.clear();
    name_.clear();
  }
  endResetModel();
}

QByteArray message_model::get_contact() const
{
  return (id_+":"+name_+":"+me_info_.id).c_str();
}

QByteArray message_model::get_contact_id() const
{
  return id_.c_str();
}

QByteArray message_model::get_contact_name() const
{
  return name_.c_str();
}

sort_message_model::sort_message_model(sms *s, const std::string& path)
  : QSortFilterProxyModel(nullptr)
  , src_(s, path)
{
  setSortOrder(false);
  setSourceModel(&src_);
  setSortRole(message_model::N_INDEX);
  setFilterRole(message_model::BODY);
}

sort_message_model::~sort_message_model()
{
}

void sort_message_model::setFilterString(QString string)
{
  this->setFilterCaseSensitivity(Qt::CaseInsensitive);
  this->setFilterFixedString(string);
}

void sort_message_model::setSortOrder(bool checked)
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

void sort_message_model::append(const QString& message)
{
  src_.append(message);
}

QByteArray sort_message_model::get_contact() const
{
  return src_.get_contact();
}

QByteArray sort_message_model::get_contact_id() const
{
  return src_.get_contact_id();
}

QByteArray sort_message_model::get_contact_name() const
{
  return src_.get_contact_name();
}

void sort_message_model::set_contact(const QByteArray& c)
{
  src_.set_contact(c);
}

message_model& sort_message_model::source()
{
  return src_;
}
