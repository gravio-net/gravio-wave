#ifndef WALLETSTORE_H
#define WALLETSTORE_H
#include "iaccount.h"
#include "secp256k1/include/secp256k1.h"
#include <vector>
#include "uint256.h"
#include "crypto/sha256.h"
#include "context.h"
#include "key.h"
#include "transactionstore.h"
#include "cryptoaddress.h"
#include "sync.h"
#include <list>

namespace gravio {
namespace wave {
namespace backend {


class Wallet: public QObject
{
    Q_OBJECT
public:
    Wallet(IAddressKeyFactory* f);
    virtual ~Wallet();
    Key NewKey();
    Context* GetContext()
    {
        return ctx;
    }
    uint64_t BlocksCount() { return txstore.BlocksCount(); }
    std::map<uint256, Transaction> GetTransactions(){ return txstore.GetTransactions(); }

public slots:
    void slotNewTransaction(uint256);
    void slotBlockCountUpdated(uint64_t);

signals:
    void newTransaction(uint256);
    void blockCountUpdated(uint64_t);

private:
    IAddressKeyFactory* factory;
    Context* ctx;
    TransactionStore txstore;
    DataSync sync;
    TransactionSync* txsync;
};

/*class WalletStore
{
public:
    WalletStore();
    Wallet NewWallet(const Currency::Type& t = Currency::GIO);
};*/

} //backend
} // wave
} // gravio

#endif // WALLETSTORE_H
