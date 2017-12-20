#ifndef WALLET_TRANSACTION_H
#define WALLET_TRANSACTION_H

#include <stdlib.h>

#include <QDateTime>

#include "../../currency.h"
#include "../../iaccount.h"
#include "../../wallet/uint256.h"
#include "../../wallet/transactionstore.h"

namespace gravio {
namespace wave {

class Wallet;

/**
 * @brief The Transaction class
 * Transaction information, actions
 */
class Transaction : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString type READ tokenizeType)
    Q_PROPERTY(QString status READ tokenizeStatus)
    Q_PROPERTY(QString address READ address)
    Q_PROPERTY(QDateTime time READ qtime)
    Q_PROPERTY(QString amount READ formatAmount)

public:
    enum Type
    {
        Other,
        Generated,
        SendToAddress,
        SendToOther,
        RecvWithAddress,
        RecvFromOther,
        SendToSelf
    };

    enum Status
    {
        Confirmed,          /**< Have 6 or more confirmations (normal tx) or fully mature (mined tx) **/
        /// Normal (sent/received) transactions
        OpenUntilDate,      /**< Transaction not yet final, waiting for date */
        OpenUntilBlock,     /**< Transaction not yet final, waiting for block */
        Offline,            /**< Not sent to any other nodes **/
        Unconfirmed,        /**< Not yet mined into a block **/
        Confirming,         /**< Confirmed, but waiting for the recommended number of confirmations **/
        Conflicted,         /**< Conflicts with other transaction or mempool **/
        Abandoned,          /**< Abandoned from the wallet **/
        /// Generated (mined) transactions
        Immature,           /**< Mined but waiting for maturity */
        MaturesWarning,     /**< Transaction will likely not mature because no nodes have confirmed */
        NotAccepted         /**< Mined but not accepted */
    };

public:
    Transaction(QObject *parent = 0);
    Transaction(Wallet* wallet, QObject *parent = 0);
    ~Transaction();

    Wallet* wallet();
    Transaction::Type type() { return type_; }
    Transaction::Status status() { return status_; }
    QString address() { return address_; }
    int64_t debit() { return debit_; }
    int64_t credit() { return credit_; }
    uint256& hash() { return hash_; }
    qint64 time() { return time_; }
    QDateTime qtime() { return QDateTime::fromTime_t(static_cast<uint>(time_)); }
    void setTime(qint64 t) { time_ = t; }
    int confirmationState();

    QString formatAmount();

    QString tokenizeStatus();
    QString tokenizeType();

    void update(backend::Transaction&);
    void create(backend::Transaction&);
    bool updateStatus();

private:
    Wallet* wallet_;
    uint256 hash_;
    qint64 time_;
    Type type_;
    QString address_;
    int64_t debit_;
    int64_t credit_;
    Status status_;
    int block_;
    int blocks_;
};

} // wave
} // gravio

#endif // WALLET_TRANSACTION_H
