#include "contacts_plugin.h"

#include <qqml.h>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlContext>

void ContactsPlugin::registerTypes(const char *)
{
    qmlRegisterType<gravio::wave::Contact>("net.gravio.wave.modules.contacts", 1, 0, "Contact");
    qmlRegisterType<gravio::wave::ContactAddress>("net.gravio.wave.modules.contacts", 1, 0, "ContactAddress");
    qmlRegisterType<gravio::wave::ContactAddresses>("net.gravio.wave.modules.contacts", 1, 0, "ContactAddresses");
}

void ContactsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    qInfo() << "[ContactsPlugin/initializeEngine]" << "Contacts model initialization...";
    QQmlExtensionPlugin::initializeEngine(engine, uri);

    engine->rootContext()->setContextProperty("contactsModel", &contacts_);
}

void ContactsPlugin::initializeModule(gravio::wave::IAccount*)
{

}
