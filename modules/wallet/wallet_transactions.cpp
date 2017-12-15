#include "wallet_transactions.h"

#include <QDateTime>

using namespace gravio::wave;

//
// Transactions
//
Transactions::Transactions(QObject *parent) : QAbstractListModel(parent)
{
    QQmlApplicationEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Transactions::Transactions(Wallet *wallet, QObject *parent) : QAbstractListModel(parent), wallet_(wallet)
{
    QQmlApplicationEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Transactions::~Transactions()
{
}

int Transactions::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return list_.size();
}

QVariant Transactions::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    Transaction *lTransaction = list_.at(index.row());

    switch (role)
    {
    case StatusRole: return lTransaction->tokenizeStatus();
    case TypeRole: return lTransaction->tokenizeType();
    case TimeRole: return QDateTime::fromTime_t(static_cast<uint>(lTransaction->time()));
    case AddressRole: return lTransaction->address();
    case AmountRole: return wallet_->units()->format(Currency::Unit::COIN, lTransaction->debit() + lTransaction->credit(), false, CurrencyUnits::separatorNever);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> Transactions::roleNames() const
{
    QHash<int, QByteArray> lRoles = QAbstractListModel::roleNames();
    lRoles[StatusRole] = "status";
    lRoles[TypeRole] = "type";
    lRoles[TimeRole] = "time";
    lRoles[AddressRole] = "address";
    lRoles[AmountRole] = "amount";

    return lRoles;
}

