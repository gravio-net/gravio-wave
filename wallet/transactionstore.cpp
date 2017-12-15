#include "transactionstore.h"
#include "datastream.h"
#include "utilstrencodings.h"
#include "amount.h"
#include <string>
#include <vector>

using namespace gravio::wave::backend;

bool TransactionStore::HasTx(std::string txid)
{
    uint256 id;
    id.SetHex(txid);
    if(txlist.find(id) != txlist.end())
        return true;
    return false;
}

void TransactionStore::AddTx(CTransaction &tx)
{
    txlist.insert(std::pair<uint256, CTransaction>(tx.GetHash(), tx));
}

CTransaction TransactionStore::CreateSendTx(int amount_val, int fee_val, std::string blob, bool subsract_fee, CryptoAddress &from_address, CryptoAddress &to_address)
{
    CMutableTransaction tx;
    CAmount amount = amount_val;
    CTxOut txout(amount, to_address.GetScript(), blob);
    tx.vout.push_back(txout);

    CAmount fee = 7500;
    std::map<uint256, CTransaction>::iterator it = txlist.begin();
    for(; it != txlist.begin(); it++)
    {
        ;
    }
    return tx;
}
