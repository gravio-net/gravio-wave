#include "wallets.h"

using namespace gravio::wave;

//
// Wallets
//
Wallets::Wallets(QObject *parent): QAbstractListModel(parent)
{
    QQmlApplicationEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Wallets::Wallets(IAccount* account, QObject *parent): QAbstractListModel(parent), account_(account)
{
    QQmlApplicationEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Wallets::~Wallets()
{
    qDebug() << "Wallets::~Wallets()";
}

int Wallets::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return wallets_.size();
}

QVariant Wallets::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    switch (role)
    {
    default:
        return QVariant();
    }
}

QVector<int> Wallets::roles() const
{
    return QVector<int>();
}

QHash<int, QByteArray> Wallets::roleNames() const
{
    QHash<int, QByteArray> lRoles = QAbstractListModel::roleNames();

    return lRoles;
}

void Wallets::initialize(IAccount* account)
{
    account_ = account;

    for(int lIdx = 0; lIdx < account_->getAddressTypes().size(); lIdx++)
    {
        Currency::Type lType = account_->getAddressTypes()[lIdx];

        Wallet* lWallet = new Wallet(account_, lType);
        lWallet->open();

        wallets_.insert(lType, lWallet);
    }
}
