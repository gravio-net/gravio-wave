#pragma once

#include <QQmlExtensionPlugin>

#include "contacts.h"

class ContactsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri) override;

private:
    ContactsViewModel contacts_;
};
