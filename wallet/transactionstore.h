#ifndef TRANSACTIONSTORE_H
#define TRANSACTIONSTORE_H

#include "context.h"
#include "cryptoaddress.h"
#include "key.h"
#include "primitives/transaction.h"
#include "iaccount.h"
#include <list>
#include <string.h>
#include <map>

namespace gravio {
namespace wave {
namespace backend {

static const unsigned int DEFAULT_MIN_RELAY_TX_FEE = 100000;
//! -maxtxfee default
static const CAmount DEFAULT_TRANSACTION_MAXFEE = 0.1 * COIN;

enum txnouttype
{
    TX_NONSTANDARD,
    // 'standard' transaction types:
    TX_PUBKEY,
    TX_PUBKEYHASH,
    TX_SCRIPTHASH,
    TX_MULTISIG,
    TX_NULL_DATA,
    TX_WITNESS_V0_SCRIPTHASH,
    TX_WITNESS_V0_KEYHASH,
};

enum isminetype
{
    ISMINE_NO = 0,
    //! Indicates that we don't know how to create a scriptSig that would solve this if we were given the appropriate private keys
    ISMINE_WATCH_UNSOLVABLE = 1,
    //! Indicates that we know how to create a scriptSig that would solve this if we were given the appropriate private keys
    ISMINE_WATCH_SOLVABLE = 2,
    ISMINE_WATCH_ONLY = ISMINE_WATCH_SOLVABLE | ISMINE_WATCH_UNSOLVABLE,
    ISMINE_SPENDABLE = 4,
    ISMINE_ALL = ISMINE_WATCH_ONLY | ISMINE_SPENDABLE
};

typedef uint8_t isminefilter;

class TransactionStore;

class Transaction: public CTransaction
{
public:
    Transaction(TransactionStore* s = 0);
    CAmount GetDebit();
    CAmount GetDebit(const isminefilter& filter);
    CAmount GetCredit();
    std::string GetAddress() { return address; }
    uint64_t GetTime() { return time; }
    void SetTime(uint64_t t) { time = t; }
    isminetype IsMine(const CTxOut& txout);

    void SetAddress(std::string addr) { address = addr; }
    bool IsTrusted();
    int GetBlocksToMaturity() const;
    int GetDepthInMainChain() const;
private:
    std::string address;
    TransactionStore* store;
    uint64_t time;
};

class COutput
{
public:
    const Transaction *tx;
    int i;
    int nDepth;
    bool fSpendable;
    bool fSolvable;

    COutput(const Transaction *txIn, int iIn, int nDepthIn, bool fSpendableIn, bool fSolvableIn)
    {
        tx = txIn; i = iIn; nDepth = nDepthIn; fSpendable = fSpendableIn; fSolvable = fSolvableIn;
    }

};
class TransactionStore
{
public:
    TransactionStore();
    TransactionStore(Context* c, IAddressKeyFactory* f);
    bool HasTx(std::string txid);
    void SetBlocksCount(uint64_t bc) { blocks_count = bc;}
    uint64_t BlocksCount() { return blocks_count; }
    void SetBalance(CAmount b) { balance = b; }
    CAmount Balance(){ return balance;}

    bool HaveKey(uint160);

    void AddTx(Transaction &tx);
    std::map<uint256, Transaction> GetTransactions(){ return txlist; }

    void AvailableCoins(std::vector<COutput> &, bool);
    void SelectCoins(const std::vector<COutput>& vAvailableCoins, const CAmount& nTargetValue, std::set<std::pair<const CTransaction*,unsigned int> >& setCoinsRet, CAmount& nValueRet);
    bool CreateSendTx(Transaction& txNew, int amount_val, int fee_val, std::string blob, bool subsract_fee, CryptoAddress &from_address, 
        CryptoAddress &to_address, int& nChangePosInOut, std::string& strFailReason);

private:
    Context* ctx;
    IAddressKeyFactory* factory;
    std::map<uint256, Transaction> txlist;
    uint64_t blocks_count;
    CAmount balance;
};

} // backend
} // wave
} // gravio

#endif // TRANSACTIONSTORE_H
