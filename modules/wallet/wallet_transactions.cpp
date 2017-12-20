#include "wallet_transactions.h"
#include "units.h"
#include "wallet.h"

#include <QDateTime>
#include <QQmlApplicationEngine>

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
    qDebug() << "Wallet::~Wallet()";

    list_.clear();
    last5list_.clear();
    timeIndex_.clear();

    qDeleteAll(transactions_.values());
    transactions_.clear();
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

    Transaction *lTransaction = list_.at(index.row()).value<Transaction *>();

    switch (role)
    {
    case StatusRole: return lTransaction->tokenizeStatus();
    case TypeRole: return lTransaction->tokenizeType();
    case TimeRole: return lTransaction->qtime();
    case AddressRole: return lTransaction->address();
    case AmountRole: return lTransaction->formatAmount();
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

QVector<int> Transactions::roles() const
{
    return QVector<int>() << StatusRole << TypeRole << TimeRole << AddressRole << AmountRole;
}

void Transactions::updateTransaction(backend::Transaction& tx)
{
    //qDebug() << "updateTransaction()" << tx.ToString().c_str();

    QMultiMap<uint256, Transaction*>::iterator lIterator = transactions_.find(tx.GetHash());
    Transaction* lTx = 0;

    if (lIterator == transactions_.end())
    {
        lTx = new Transaction(wallet_);
        lTx->create(tx);
        if (!lTx->time()) lTx->setTime(getTime());

        transactions_.insert(tx.GetHash(), lTx);
        timeIndex_.insert(lTx->time(), lTx);
    }
    else
    {
        lTx = lIterator.value();
        lTx->update(tx);
    }

    beginInsertRows(QModelIndex(), 0, 0); // first place

    list_.insert(0, QVariant::fromValue((--(timeIndex_.end())).value()));
    last5list_.insert(0, QVariant::fromValue((--(timeIndex_.end())).value())); // fill list

    if (last5list_.size() >= 5) last5list_.pop_back();

    endInsertRows();
}

void Transactions::updateBlockCount()
{
    bool lReset = false;
    for(int lIdx = 0; lIdx < list_.size(); lIdx++)
    {
        Transaction* lTx = list_[lIdx].value<Transaction*>();
        if (lTx->updateStatus())
        {
            lReset = true;
        }
    }

    if (lReset)
    {
        beginResetModel();
        endResetModel();
    }
}

void Transactions::backendInitialized()
{
    // fill entire model
}
