#ifndef WALLETSTORE_H
#define WALLETSTORE_H
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
    //Q_OBJECT
public:
    Wallet(const Currency::Type & t = Currency::GIO);
    virtual ~Wallet() {}
    Key NewKey();
    void SetKey(Key& k);
    void AddPubKey(PubKey& pk);
    Context* GetContext()
    {
        return &ctx;
    }
private:
    Context ctx;
    KeyStore keystore;
    TransactionStore txstore;
    DataSync sync;
    TransactionSync txsync;
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
