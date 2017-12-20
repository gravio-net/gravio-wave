#ifndef WALLETS_H
#define WALLETS_H

#include <QQuickItem>
#include <QAbstractListModel>
#include <QList>

#include "../../currency.h"
#include "../../iaccount.h"
#include "wallet.h"

namespace gravio {
namespace wave {

/**
 * @brief The Wallets class
 * Wallets model
 */
class Wallets: public QAbstractListModel
{
    Q_OBJECT
public:
    Wallets(QObject *parent = 0);
    Wallets(IAccount* account, QObject *parent = 0);
    ~Wallets();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;
    QVector<int> roles() const;

    QList<Wallet*> getList() const
    {
        return wallets_.values();
    }

    Q_INVOKABLE int count() { return wallets_.size(); }
    Q_INVOKABLE QVariant get(int index)
    {
        Wallet* lWallet = wallets_.values().at(index);
        return QVariant::fromValue(lWallet);
    }

    void initialize(IAccount*);

signals:
    void listChanged();

private:
    IAccount* account_;
    QMap<Currency::Type, Wallet*> wallets_;
};

} // wave
} // gravio

#endif // WALLETS_H
