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

class Transaction: public CTransaction
{
public:
    CAmount GetDebit() { return 0; }
    CAmount GetCredit() { return 0; }
    std::string GetAddress() { return address; }
    uint64_t GetTime() { return 0; }

    SetAddress(std::string addr) { address = addr; }
private:
    std::string address;
};

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

    void AddTx(Transaction &tx);
    std::map<uint256, Transaction> GetTransactions(){ return txlist; }
    Transaction CreateSendTx(int amount_val, int fee_val, std::string blob, bool subsract_fee, CryptoAddress &from_address, CryptoAddress &to_address);

private:
    Context* ctx;
    std::map<uint256, Transaction> txlist;
    uint64_t blocks_count;
    CAmount balance;
};

} // backend
} // wave
} // gravio

#endif // TRANSACTIONSTORE_H
