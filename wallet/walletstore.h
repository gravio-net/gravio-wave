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
#include <list>

using namespace gravio::wave;

class Wallet
{
public:
    Wallet(const Currency::Type&);
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
};

class WalletStore
{
public:
    WalletStore();
    Wallet NewWallet(const Currency::Type& t = Currency::GIO);
};

#endif // WALLETSTORE_H
