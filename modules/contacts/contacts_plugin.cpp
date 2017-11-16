#include "contacts_plugin.h"

#include <qqml.h>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlContext>

void ContactsPlugin::registerTypes(const char *uri)
{
    qInfo() << "[ContactsPlugin/registerTypesEngine]" << "ContactsViewModel registered with" << URI;
    qmlRegisterType<ContactsViewModel>(URI, 1, 0, NAME);
}

void ContactsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    qInfo() << "[ContactsPlugin/initializeEngine]" << "ContactsViewModel initialization...";
    QQmlExtensionPlugin::initializeEngine(engine, uri);

    engine->rootContext()->setContextProperty("contactsModel", &contacts_);
}
