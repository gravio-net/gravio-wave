#include "transactionstore.h"
#include "datastream.h"
#include "utilstrencodings.h"
#include "amount.h"
#include <string>
#include <vector>

using namespace gravio::wave::backend;

Transaction::Transaction(TransactionStore* s):store(s)
{
    ;
}

CAmount Transaction::GetDebit()
{
    CAmount debit = 0;
    for(std::vector<CTxIn>::const_iterator it = this->vin.begin(); it != this->vin.end(); it++)
    {
        CTxIn txin = *it;
        txin.prevout.hash;
    }
    return debit;
}

TransactionStore::TransactionStore() : ctx(0)
{

}

TransactionStore::TransactionStore(Context* c) : ctx(c)
{

}

bool TransactionStore::HasTx(std::string txid)
{
    uint256 id;
    id.SetHex(txid);
    if(txlist.find(id) != txlist.end())
        return true;
    return false;
}

void TransactionStore::AddTx(Transaction &tx)
{
    txlist.insert(std::pair<uint256, Transaction>(tx.GetHash(), tx));
}

Transaction TransactionStore::CreateSendTx(int amount_val, int fee_val, std::string blob, bool subsract_fee, CryptoAddress &from_address, CryptoAddress &to_address)
{
    Transaction tx;
    CMutableTransaction mtx;
    CAmount amount = amount_val;
    CTxOut txout(amount, to_address.GetScript(), blob);
    //tx.vout.push_back(txout);

    //CAmount fee = 7500;
    //std::map<uint256, CTransaction>::iterator it = txlist.begin();
    //for(; it != txlist.begin(); it++)
    //{
    //    ;
    //}
    return tx;
}
