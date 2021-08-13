#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QLoggingCategory>

#include "simplemessage.h"
#include "simplemessage_ev.h"
#include "contact_model.h"
#include "message_model.h"

#include "misc_log_ex.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "wallet.simplemessage"

namespace cryptonote {

  void simplemessage::run()
  {
#ifdef QT_NO_DEBUG_OUTPUT
    QLoggingCategory::setFilterRules("*.debug=false\n"
                                     "*.info=false\n"
                                     "*.warning=false\n"
                                     "*.critical=true");
#endif

    int argc = 1;
    char *argv[] = { (char *)"simplemessage" };

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qRegisterMetaType<std::string>();
    
    qmlRegisterUncreatableType<sort_contact_model>("ContactModelType", 1, 0, "ContactModelType", "contact_model can't be instantiated directly");
    qmlRegisterUncreatableType<sort_message_model>("MessageModelType", 1, 0, "MessageModelType", "nessage_model can't be instantiated directly");
    
    sort_contact_model c(cc_, sec_, path_);
    sort_message_model m(sec_, path_);

    c.source().set_self_id(self_id_);
    c.source().set_self_name(self_name_);
    m.source().set_self_id(self_id_);
    m.source().set_self_name(self_name_);

    engine.rootContext()->setContextProperty("contact_model", &c);
    engine.rootContext()->setContextProperty("message_model", &m);

    simplemessage_ev ev;
    
    QObject::connect(&ev, &simplemessage_ev::c_add, &c.source(), &contact_model::add);
    QObject::connect(&ev, &simplemessage_ev::c_remove, &c.source(), &contact_model::remove);

    const QUrl url(QStringLiteral("qrc:/simplemessage.qml"));
    engine.load(url);
    
    p_App = &app;
    p_Ev = &ev;
    
    app.exec();
  }

  void simplemessage::stop()
  {
    p_App->quit();
  }
  
  void simplemessage::c_initialize(const std::vector<std::pair<std::string, std::string>>& data)
  {
    cc_ = data;
  }
  
  void simplemessage::c_add(const std::string& id, const std::string& name)
  {
    emit p_Ev->c_add(id, name);
  }

  void simplemessage::c_remove(const std::string& id)
  {
    emit p_Ev->c_remove(id);
  }
}
