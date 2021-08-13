#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QLoggingCategory>

#include "main.h"
#include "contact_model.h"
#include "message_model.h"

#include "misc_log_ex.h"

#include <thread>

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "wallet.sms"

int main(int argc, char *argv[])
{
  boost::program_options::options_description desc("Options");
  boost::program_options::options_description_easy_init opt = desc.add_options();

  opt("zyre-group", boost::program_options::value<std::vector<std::string>>(), "zyre group");
  opt("zyre-lan", boost::program_options::value<std::string>(), "zyre lan");
  opt("data-path", boost::program_options::value<std::string>(), "data path");
  opt("log-file", boost::program_options::value<std::string>(), "log file");

  // default group is sms-server
  std::vector<std::string> groups = {"sms-server"};
  // default lan is gyro-public
  std::string lan = "gyro-public";
  // database of sms
  std::string path = "./gyro-sms.dat";
  // default group is sms-server
  std::string log_file = "gyro-sms.log";

  boost::program_options::variables_map vm;

  boost::program_options::store(
      boost::program_options::parse_command_line(argc, argv, desc), vm);

  if(vm.count("zyre-group") > 0) groups = vm["zyre-group"].as<std::vector<std::string>>();
  if(vm.count("zyre-lan") > 0) lan = vm["zyre-lan"].as<std::string>();
  if(vm.count("data-path") > 0) path = vm["data-path"].as<std::string>();
  if(vm.count("log-file") > 0) log_file = vm["log-file"].as<std::string>();

  mlog_configure(log_file, true);

#ifdef QT_NO_DEBUG_OUTPUT
  QLoggingCategory::setFilterRules("*.debug=false\n"
                                   "*.info=false\n"
                                   "*.warning=false\n"
                                   "*.critical=true");
#endif

  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;

  sms s(groups, path, lan);

  s.start();

  qRegisterMetaType<std::string>();
  
  qmlRegisterUncreatableType<sort_contact_model>("ContactModelType", 1, 0, "ContactModelType", "contact_model can't be instantiated directly");
  qmlRegisterUncreatableType<sort_message_model>("MessageModelType", 1, 0, "MessageModelType", "nessage_model can't be instantiated directly");
  
  sort_contact_model c(&s, path);
  sort_message_model m(&s, path);

  engine.rootContext()->setContextProperty("contact_model", &c);
  engine.rootContext()->setContextProperty("message_model", &m);

  QObject::connect(&s, &sms::contact_add, &c.source(), &contact_model::add);
  QObject::connect(&s, &sms::contact_remove, &c.source(), &contact_model::remove);
  QObject::connect(&s, &sms::message_received_from, &m.source(), &message_model::on_received_from);
  QObject::connect(&m.source(), &message_model::contacts_invalidate, &c, &sort_contact_model::invalidate);
  //QObject::connect(&s, &sms::message_changed, &m.source(), &message_model::on_changed);

  const QUrl url(QStringLiteral("qrc:/main.qml"));
  engine.load(url);

  s.zyre().call_r(SHOUT, s.zyre().groups(), "sms_get_wallets",
      zyre::fwrap(&sms::put_wallet, &s));

  int rc = app.exec();

  s.stop();

  return rc;
}

sms::sms(const std::vector<std::string>& groups, const std::string& path, const std::string& pin)
  : QObject(nullptr)
  , data_(path)
  , zsrv_(GYRO_SMS, groups, pin)
  , zyre_(ios_, GYRO_SMS, zsrv_)
  , groups_(groups)
{
  zyre_.on("sms_put_contact", zyre::fwrap(&sms::put_contact, this));
  zyre_.on("sms_del_contact", zyre::fwrap(&sms::del_contact, this));
  zyre_.on("sms_put_message", zyre::fwrap(&sms::put_message, this));
}

sms::~sms()
{
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
    emit contact_add(c.address, c.name, c.key, me);
}

void sms::put_contact(const std::string& me, const zyre::wallet::contact_t& c)
{
  emit contact_add(c.address, c.name, c.key, me);
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
  emit contact_remove(id);
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
  emit message_received_from(msg.ta, int(n), msg.fa);
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
