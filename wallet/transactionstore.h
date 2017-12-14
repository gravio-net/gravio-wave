#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "context.h"
#include "cryptoaddress.h"
#include "key.h"
#include "primitives/transaction.h"
#include <list>
#include <string.h>
#include <map>

namespace gravio {
namespace wave {
namespace backend {

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
    void SetBlocksCount(size_t bc) { blocks_count = bc;}
    void AddTx(CTransaction &tx);
    CTransaction CreateSendTx(int amount_val, int fee_val, std::string blob, bool subsract_fee, CryptoAddress &from_address, CryptoAddress &to_address);

private:
    Context* ctx;
    KeyStore* keystore;
    std::map<uint256, CTransaction> txlist;
    size_t blocks_count;
};

} //backend
} // wave
} // gravio

#endif // TRANSACTION_H
