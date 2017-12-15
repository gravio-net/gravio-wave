#include "wallet_plugin.h"

#include <qqml.h>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlContext>

void WalletPlugin::registerTypes(const char *)
{
    qmlRegisterType<gravio::wave::Wallet>("net.gravio.wave.modules.wallet", 1, 0, "Wallet");
    qmlRegisterType<gravio::wave::Wallets>("net.gravio.wave.modules.wallet", 1, 0, "Wallets");
}

void WalletPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    qInfo() << "[WalletPlugin/initializeEngine]" << "Wallets model registration...";
    QQmlExtensionPlugin::initializeEngine(engine, uri);

    engine->rootContext()->setContextProperty("walletsModel", &wallets_);
}

void WalletPlugin::initializeModule(gravio::wave::IAccount* account)
{
    qInfo() << "[WalletPlugin/initialize]" << "Wallets model initialization...";
    wallets_.initialize(account);
}
