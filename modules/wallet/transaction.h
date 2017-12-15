#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <stdlib.h>

#include "../../currency.h"
#include "../../iaccount.h"
#include "../../wallet/uint256.h"
#include "wallet.h"

namespace gravio {
namespace wave {

/**
 * @brief The Transaction class
 * Transaction information, actions
 */
class Transaction : public QObject
{
    Q_OBJECT
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

    Wallet* wallet() { return wallet_; }
    Transaction::Type type() { return type_; }
    Transaction::Status status() { return status_; }
    QString address() { return address_; }
    int64_t debit() { return debit_; }
    int64_t credit() { return credit_; }
    uint256& hash() { return hash_; }
    qint64 time() { return time_; }
    int confirmationState()
    {
        double lTotal = wallet_->factory()->confirmations();
        int lPercent = (int)((double)blocks_ * 100.0) / lTotal;
        return lPercent;
    }

    QString tokenizeStatus();
    QString tokenizeType();

private:
    Wallet* wallet_;
    uint256 hash_;
    qint64 time_;
    Type type_;
    QString address_;
    int64_t debit_;
    int64_t credit_;
    Status status_;
    int blocks_;
};

} // wave
} // gravio

#endif // TRANSACTION_H
