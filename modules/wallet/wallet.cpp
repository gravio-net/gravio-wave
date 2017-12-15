#include "wallet.h"

using namespace gravio::wave;

//
// Wallet
//
Wallet::Wallet(QObject *parent) : QObject(parent)
{
    units_ = 0;
    factory_ = 0;
    opened_ = false;

    QQmlApplicationEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Wallet::Wallet(IAccount* account, Currency::Type type, QObject *parent) : QObject(parent), account_(account), type_(type)
{
    units_ = 0;
    factory_ = 0;
    opened_ = false;

    QQmlApplicationEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Wallet::~Wallet()
{
    if (units_) delete units_;
}

void Wallet::open()
{
    if (!opened_)
    {
        factory_ = account_->getAddressFactory(type_);
        units_ = new CurrencyUnits(factory_);

        opened_ = true;
    }
}

QString Wallet::availableBalance()
{
    return QString("0.00000000 ") + QString::fromStdString(Currency::name(type_));
}

QString Wallet::pendingBalance()
{
    return QString("0.00000000 ") + QString::fromStdString(Currency::name(type_));
}

QString Wallet::totalBalance()
{
    return QString("0.00000000 ") + QString::fromStdString(Currency::name(type_));
}

