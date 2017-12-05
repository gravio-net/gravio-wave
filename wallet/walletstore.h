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

class Wallet
{
public:
    Wallet(const WalletType&);
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
    Wallet NewWallet(const WalletType& t = WalletType::GIO);
};

#endif // WALLETSTORE_H
