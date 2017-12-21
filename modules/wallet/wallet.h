#ifndef WALLET_H
#define WALLET_H

#include <QString>
#include <QList>
#include <QAbstractListModel>
#include <QQuickItem>
#include <QMap>
#include <QQmlApplicationEngine>

#include "../../currency.h"
#include "../../iaccount.h"
#include "units.h"
#include "../../wallet/walletstore.h"
#include "wallet_transactions.h"

namespace gravio {
namespace wave {

/**
 * @brief The Wallet class
 * Wallet information, actions and models (transations)
 */
class Wallet : public QObject
{
    Q_OBJECT
public:
    Wallet(QObject *parent = 0);
    Wallet(IAccount* account, Currency::Type type, QObject *parent = 0);
    ~Wallet();

    void open();

    Q_INVOKABLE QString addressType() { return QString(Currency::name(type_).c_str()); }

    Q_INVOKABLE QString availableBalance();
    Q_INVOKABLE QString pendingBalance();
    Q_INVOKABLE QString totalBalance();
    Q_INVOKABLE QString ccy() { return QString::fromStdString(Currency::name(type_)); }
    Q_INVOKABLE QVariant transactions() { return QVariant::fromValue(transactions_); }

    IAddressKeyFactory* factory() { return factory_; }
    CurrencyUnits* units() { return units_; }
    uint64_t blocksCount() { return backEnd_->BlocksCount(); }

public slots:
    void transactionUpdated(uint256);
    void blockCountUpdated(uint64_t);

private:
    Currency::Type type_;
    CurrencyUnits* units_;
    IAccount* account_;
    IAddressKeyFactory* factory_;
    Transactions* transactions_;
    backend::Wallet* backEnd_;
    bool opened_;
};

} // wave
} // gravio

#endif // WALLET_H
