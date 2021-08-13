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

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
  std::vector<std::string> groups;
  std::string net, pin, path, log_file;

  po::options_description desc("options");
  desc.add_options()
    ("zyre-group"   ,po::value(&groups)                                     ,"zyre group"       )
    ("zyre-net"     ,po::value(&net)->default_value("gyro-public")          ,"zyre net"         )
    ("zyre-pin"     ,po::value(&pin)                                        ,"zyre pin"         )
    ("data-path"    ,po::value(&path)->default_value("./gyro-sms.dat")      ,"data path"        )
    ("log-file"     ,po::value(&log_file)->default_value("gyro-sms.log")    ,"log file"         );

    po::command_line_parser parser{argc, argv};
    parser.options(desc);
    po::parsed_options parsed_options = parser.run();
    
    po::variables_map vm;
    po::store(parsed_options, vm);
    po::notify(vm);

  mlog_configure(log_file, true);

#ifdef QT_NO_DEBUG_OUTPUT
  QLoggingCategory::setFilterRules("*.debug=false\n"
                                   "*.info=false\n"
                                   "*.warning=false\n"
                                   "*.critical=true");
#endif

  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;

  boost::asio::io_service ios;
  sms2 s(ios, groups, path, net+pin);

  s.start();

  qRegisterMetaType<std::string>();
  
  qmlRegisterUncreatableType<sort_contact_model>("ContactModelType", 1, 0, "ContactModelType", "contact_model can't be instantiated directly");
  qmlRegisterUncreatableType<sort_message_model>("MessageModelType", 1, 0, "MessageModelType", "nessage_model can't be instantiated directly");
  
  sort_contact_model c(&s, path);
  sort_message_model m(&s, path);

  engine.rootContext()->setContextProperty("contact_model", &c);
  engine.rootContext()->setContextProperty("message_model", &m);

  QObject::connect(&s, &sms2::contact_add_2, &c.source(), &contact_model::add);
  QObject::connect(&s, &sms2::contact_remove_2, &c.source(), &contact_model::remove);
  QObject::connect(&s, &sms2::message_received_from_2, &m.source(), &message_model::on_received_from);
  QObject::connect(&m.source(), &message_model::contacts_invalidate, &c, &sort_contact_model::invalidate);
  //QObject::connect(&s, &sms::message_changed, &m.source(), &message_model::on_changed);

  const QUrl url(QStringLiteral("qrc:/main.qml"));
  engine.load(url);

  s.zyre().call_r(SHOUT, s.zyre().groups(), "sms_get_wallet",
      zyre::fwrap(&sms::put_wallet, static_cast<sms*>(&s)));

  int rc = app.exec();

  s.stop();

  return rc;
}

sms2::sms2(boost::asio::io_service& ios, const std::vector<std::string>& groups, const std::string& path, const std::string& pin)
  : sms(ios, groups, path, pin), QObject(nullptr)
{
}

void sms2::contact_add(const std::string& id, const std::string& name, const std::string& key, const std::string& me)
{
  emit contact_add_2(id, name, key, me);
}

void sms2::contact_remove(const std::string& id)
{
  emit contact_remove_2(id);
}

void sms2::message_received_from(const std::string& to, int n, const std::string& from)
{
  emit message_received_from_2(to, n, from);
}
