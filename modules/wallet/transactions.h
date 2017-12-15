#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

#include <QMultiMap>

#include "transaction.h"

namespace gravio {
namespace wave {

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

    QList<Transaction*> getList() const
    {
        return list_;
    }

    Q_INVOKABLE int count() { return list_.size(); }
    Q_INVOKABLE QVariant get(int index)
    {
        Transaction* lTransaction = list_.at(index);
        return QVariant::fromValue(lTransaction);
    }

signals:
    void listChanged();

private:
    Wallet* wallet_;
    QList<Transaction*> list_;
    QMultiMap<qint64, uint256> timeIndex_;
    QMultiMap<uint256, Transaction*> transactions_;
};

} // wave
} // gravio

#endif // TRANSACTIONS_H
