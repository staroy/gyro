#include "message_model.h"

#include <cstdlib>
#include <ctime>

#include <QDateTime>

#include "misc_log_ex.h"
#include "string_tools.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "wallet.simplemessage"

message_model::message_model(const crypto::secret_key& sec, const std::string& path)
  : QAbstractListModel(nullptr)
  , sec_(sec)
  , data_(path)
{
  crypto::secret_key_to_public_key(sec_, pub_);
  salt_ = gyro::sms::sync::get_salt(sec_);
}

message_model::~message_model()
{
}

int message_model::rowCount(const QModelIndex &) const
{
  auto data = data_;
  std::string tmp = current_id_ + salt_;
  gyro::sms::hash_t h_current;
  crypto::cn_fast_hash(tmp.data(), tmp.size(), h_current.u.h);
  uint64_t n;
  if(!(data[CONTACT][h_current][MESSAGES_COUNTER] >> n))
  return 0;
  return qulonglong(n);
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
  roles[SENDED] = "sended";
  return roles;
}

QVariant message_model::data(const QModelIndex &index, int role) const
{
  static std::unordered_map<uint64_t, gyro::sms::data_t> heap;
  static std::string current_id;

  if(!index.isValid())
    return QVariant();
  
  if(current_id != current_id_)
  {
    heap.clear();
    current_id = current_id_;
  }
  
  auto data = data_;

  std::string tmp = current_id_ + salt_;
  gyro::sms::hash_t h_current;
  crypto::cn_fast_hash(tmp.data(), tmp.size(), h_current.u.h);
  
  gyro::leveldb::cxx::db current_data = data[MESSAGE][h_current];
  
  gyro::sms::data_t *p_info = nullptr;
  uint64_t n;
  int row = index.row();

  auto it = current_data.begin() += row;
  if(it != current_data.end())
  {
    gyro::sms::hash_t h_from; uint64_t f;
    it->first >> n >> h_from >> f;
    
    if(heap.find(n) == heap.end())
    {
      gyro::sms::data_cipher_t cipher;
      it->second >> cipher;

      std::string buf;
      try {
        gyro::sms::sync::decrypt(cipher.d, cipher.k, cipher.iv, sec_, buf);
      } catch (const std::exception& e) {
        MLOG_RED(el::Level::Warning, "error decrypt: " << e.what());
        return QVariant();
      }

      gyro::sms::data_t info;
      try {
        msgpack::object_handle o1 = msgpack::unpack(buf.data(), buf.size());
        o1.get().convert(info);
      } catch (const std::exception& e) {
        MLOG_RED(el::Level::Warning, "error deserialize: " << e.what());
        return QVariant();
      }

      heap[n] = info;
    }
    p_info = &heap[n];
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
      case SENDED: return true;
    }
  }

  return QVariant();
}

void message_model::append(const QString& message)
{
  if(current_id_.size() > 0)
  {
    std::string tmp = current_id_ + salt_;
    gyro::sms::hash_t h_to;
    crypto::cn_fast_hash(tmp.data(), tmp.size(), h_to.u.h);
    
    tmp = self_id_ + salt_;
    gyro::sms::hash_t h_from;
    crypto::cn_fast_hash(tmp.data(), tmp.size(), h_from.u.h);

    time_t tm = time(NULL);
    
    gyro::sms::data_t info{ message.toUtf8().constData(), current_name_, current_id_, self_name_, self_id_, tm };
    std::stringstream ss;
    msgpack::pack(ss, info);
    gyro::sms::data_cipher_t cipher;
    try {
      gyro::sms::encrypt(pub_, ss.str(), cipher.d, cipher.k, cipher.iv);
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
    endInsertRows();
    emit do_send_to(current_id_, int(n));
  }
}

void message_model::on_received_from(std::string to, int n_index, std::string from)
{
  if((to == self_id_ && from == current_id_) || to == current_id_)
  {
    beginInsertRows(QModelIndex(), n_index, n_index);
    endInsertRows();
    emit contacts_invalidate();
  }
}

void message_model::on_changed(std::string to, int n_index)
{
  if(to == current_id_)
    emit dataChanged(index(n_index,0), index(n_index,0));
}

QByteArray message_model::get_current_id() const
{
  return current_id_.c_str();
}

void message_model::set_current_id(const QByteArray& id)
{
  beginResetModel();
  if(id.length()>0)
    current_id_ = id;
  else
    current_id_.clear();
  current_name_.clear();
  endResetModel();
}

void message_model::set_current_contact(const QByteArray& id, const QByteArray& name)
{
  beginResetModel();
  if(id.length()>0)
  {
    current_id_ = id;
    current_name_ = name;
  }
  else
  {
    current_id_.clear();
    current_name_.clear();
  }
  endResetModel();
}

sort_message_model::sort_message_model(const crypto::secret_key& sec, const std::string& data_dir)
  : QSortFilterProxyModel(nullptr)
  , src_(sec, data_dir)
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

QByteArray sort_message_model::get_current_id() const
{
  return src_.get_current_id();
}

void sort_message_model::set_current_id(const QByteArray& id)
{
  src_.set_current_id(id);
}

void sort_message_model::set_current_contact(const QByteArray& id, const QByteArray& name)
{
  src_.set_current_contact(id, name);
}

message_model& sort_message_model::source()
{
  return src_;
}
