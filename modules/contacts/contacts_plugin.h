#pragma once

#include <QQmlExtensionPlugin>

#include "contacts.h"
#include "../../imoduleplugin.h"

class ContactsPlugin : public QQmlExtensionPlugin, public gravio::wave::IModulePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri) override;
    void initializeModule(gravio::wave::IAccount*) override;

private:
    gravio::wave::Contacts contacts_;
};
