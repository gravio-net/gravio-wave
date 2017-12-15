#ifndef TRANSACTIONSTORE_H
#define TRANSACTIONSTORE_H

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

class TransactionStore
{
public:
    TransactionStore();
    TransactionStore(Context* c);
    bool HasTx(std::string txid);
    void SetBlocksCount(uint64_t bc) { blocks_count = bc;}
    uint64_t BlocksCount() { return blocks_count; }
    void SetBalance(CAmount b) { balance = b; }
    CAmount Balance(){ return balance;}

    void AddTx(CTransaction &tx);
    std::map<uint256, CTransaction> GetTransactions(){ return txlist; }
    CTransaction CreateSendTx(int amount_val, int fee_val, std::string blob, bool subsract_fee, CryptoAddress &from_address, CryptoAddress &to_address);

private:
    Context* ctx;
    std::map<uint256, CTransaction> txlist;
    uint64_t blocks_count;
    CAmount balance;
};

} // backend
} // wave
} // gravio

#endif // TRANSACTIONSTORE_H
