#ifndef TRANSACTIONSTORE_H
#define TRANSACTIONSTORE_H

#include "context.h"
#include "cryptoaddress.h"
#include "key.h"
#include "primitives/transaction.h"
#include <list>
#include <string.h>
#include <map>

class Transaction
{
public:
    Transaction();
private:
    uint256 hash;
};

class TransactionStore
{
public:
    TransactionStore();
    TransactionStore(Context* c, KeyStore* k);
    bool HasTx(std::string txid);
    void AddTx(CTransaction &tx);
    CTransaction CreateSendTx(int amount_val, int fee_val, std::string blob, bool subsract_fee, CryptoAddress &from_address, CryptoAddress &to_address);

private:
    Context* ctx;
    KeyStore* keystore;
    std::map<uint256, CTransaction> txlist;
};

#endif // TRANSACTIONSTORE_H
