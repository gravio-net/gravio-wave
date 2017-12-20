#ifndef WALLET_TRANSACTIONS_H
#define WALLET_TRANSACTIONS_H

#include "wallet_transaction.h"

#include <QMultiMap>
#include <QVariantList>
#include <QAbstractListModel>

namespace gravio {
namespace wave {

class Wallet;

/**
 * @brief The Transactions class
 * Transactions model
 */
class Transactions: public QAbstractListModel
{
    Q_OBJECT
public:
    enum TransactionsRoles
    {
        StatusRole = Qt::UserRole + 1,
        TypeRole,
        TimeRole,
        AddressRole,
        AmountRole
    };

    Transactions(QObject *parent = 0);
    Transactions(Wallet* wallet, QObject *parent = 0);
    ~Transactions();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;
    QVector<int> roles() const;

    Q_INVOKABLE QVariantList getList() const
    {
        return list_;
    }
    Q_INVOKABLE QVariantList getLast5List() const
    {
        return last5list_;
    }

    Q_INVOKABLE int count() { return list_.size(); }
    Q_INVOKABLE QVariant get(int index)
    {
        return list_.at(index);
    }

    void updateTransaction(backend::Transaction&);
    void updateBlockCount();
    void backendInitialized();

signals:
    void listChanged();

private:
    int64_t getTime()
    {
        time_t now = time(NULL);
        return now;
    }

private:
    Wallet* wallet_;
    QVariantList list_;
    QVariantList last5list_;
    QMultiMap<qint64, Transaction*> timeIndex_;
    QMultiMap<uint256, Transaction*> transactions_;
};

} // wave
} // gravio

#endif // WALLET_TRANSACTIONS_H
