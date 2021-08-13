
#include "sms.h"
#include "misc_log_ex.h"
#include <thread>

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "wallet.sms"

sms::sms(boost::asio::io_service& ios, const std::vector<std::string>& groups, const std::string& path, const std::string& pin)
  : ios_(ios)
  , data_(path)
  , zsrv_(GYRO_SMS, groups, pin)
  , zyre_(ios_, GYRO_SMS, zsrv_)
  , groups_(groups)
{
  zyre_.on("sms_put_contact", zyre::fwrap(&sms::put_contact, this));
  zyre_.on("sms_del_contact", zyre::fwrap(&sms::del_contact, this));
  zyre_.on("sms_put_message", zyre::fwrap(&sms::put_message, this));
}

void sms::start()
{
  zsrv_.start();
  zyre_.start();
  zyre_.s_join(GYRO_SMS);
  for(auto g : groups_)
    zyre_.join(g);
  thread_ = std::thread([&](){ ios_.run(); });
}

void sms::stop()
{
  zyre_.s_leave(GYRO_SMS);
  zyre_.stop();
  ios_.stop();
  zsrv_.stop();
  zsrv_.join();
  thread_.join();
}

void sms::put_contacts(const std::string& me, const zyre::wallet::contacts_t& a)
{
  for(const auto& c : a)
    contact_add(c.address, c.name, c.key, me);
}

void sms::put_contact(const std::string& me, const zyre::wallet::contact_t& c)
{
  contact_add(c.address, c.name, c.key, me);
  boost::unique_lock<boost::mutex> lock(m_me_mutex_);
  me_info_t& nfo = me_[me];
  std::string tmp = c.address + nfo.salt;
  zyre::wallet::hash_t h_from;
  crypto::cn_fast_hash(tmp.data(), tmp.size(), h_from.u.h);
  uint64_t n = 0;
  data_[CONTACT][h_from][MESSAGES_COUNTER] >> n;
  zyre().call_r(SHOUT, zyre().groups(), "sms_get_messages",
    zyre::fwrap(&sms::put_messages, this), h_from, n, uint64_t(-1));
}

void sms::del_contact(const std::string& /*me*/, const std::string& id)
{
  contact_remove(id);
}

void sms::put_messages(const std::string& me, const std::vector<std::pair<uint64_t, zyre::wallet::data_cipher_t>>& mm)
{
  for(const auto& m : mm)
    put_message(me, m.first, m.second);
}

void sms::put_message(const std::string& me, const uint64_t& n, const zyre::wallet::data_cipher_t& cipher)
{
  std::string tmp, salt;
  try {
    boost::unique_lock<boost::mutex> lock(m_me_mutex_);
    me_info_t& nfo = me_[me];
    zyre::wallet::decrypt(cipher.d, cipher.u.k, cipher.iv, nfo.sec, tmp);
    salt = nfo.salt;
  } catch (const std::exception& e) {
    MLOG_RED(el::Level::Warning, "failed decript message for send: " << e.what());
    return;
  }
  zyre::wallet::data_t msg;
  msgpack::object_handle v = msgpack::unpack(tmp.data(), tmp.size());
  v.get().convert(msg);
  zyre::wallet::hash_t h_to; tmp = msg.ta + salt;
  crypto::cn_fast_hash(tmp.data(), tmp.size(), h_to.u.h);
  zyre::wallet::hash_t h_from; tmp = msg.fa + salt;
  crypto::cn_fast_hash(tmp.data(), tmp.size(), h_from.u.h);
  data_[MESSAGE][h_from][n][h_to][MESSAGE_RECEIVED] = cipher;
  data_[CONTACT][h_from][MESSAGES_COUNTER] = n;
  data_[CONTACT][h_from][MESSAGES_TIME] = msg.ts;
  message_received_from(msg.ta, int(n), msg.fa);
}

void sms::put_wallet(const std::string& id, const std::string& name, const zyre::wallet::sec_t& sec)
{
  boost::unique_lock<boost::mutex> lock(m_me_mutex_);
  me_info_t& nfo = me_[id];
  nfo.id = id;
  nfo.name = name;
  //if(!epee::string_tools::hex_to_pod(sec, nfo.sec)) {
    //MLOG_RED(el::Level::Warning, "error hex_to_pod: " << sec);
    //return;
  //}
  std::memcpy(nfo.sec.data, sec.u.k.data, sizeof(nfo.sec.data));
  nfo.pub = zyre::wallet::get_sms_public_key(nfo.sec);
  nfo.salt = zyre::wallet::get_sms_salt(nfo.sec);
  zyre().call_r(SHOUT, zyre().groups(), "sms_get_contacts",
    zyre::fwrap(&sms::put_contacts, this));
}

//void sms::do_send_to(std::string to, int n, std::string me)
//{
//  me_info_t& nfo = me_[me];
//  zyre::wallet::hash_t h_to; std::string tmp = to + nfo.salt;
//  crypto::cn_fast_hash(tmp.data(), tmp.size(), h_to.u.h);
//  zyre::wallet::hash_t h_from; tmp = me + nfo.salt;
//  crypto::cn_fast_hash(tmp.data(), tmp.size(), h_from.u.h);
//  zyre::wallet::data_cipher_t cipher;
//  data_[MESSAGE][h_to][n][h_from][MESSAGE_TOSEND] >> cipher;
//}

void sms::join(const std::string& name)
{
  zyre_.join(name);
}

void sms::leave(const std::string& name)
{
  zyre_.leave(name);
}
