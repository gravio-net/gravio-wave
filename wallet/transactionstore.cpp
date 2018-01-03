#include "transactionstore.h"
#include "datastream.h"
#include "utilstrencodings.h"
#include "amount.h"
#include <string>
#include <vector>
#include <map>

using namespace gravio::wave::backend;
using namespace std;

CFeeRate minRelayTxFee = CFeeRate(DEFAULT_MIN_RELAY_TX_FEE);
CAmount maxTxFee = DEFAULT_TRANSACTION_MAXFEE;

/** Coinbase transaction outputs can only be spent after this number of new blocks (network rule) */
static const int COINBASE_MATURITY = 100;

typedef vector<unsigned char> valtype;

/**
 * Return public keys or hashes from scriptPubKey, for 'standard' transaction types.
 */
bool Solver(const CScript& scriptPubKey, txnouttype& typeRet, vector<vector<unsigned char> >& vSolutionsRet)
{
    // Templates
    static multimap<txnouttype, CScript> mTemplates;
    if (mTemplates.empty())
    {
        // Standard tx, sender provides pubkey, receiver adds signature
        mTemplates.insert(make_pair(TX_PUBKEY, CScript() << OP_PUBKEY << OP_CHECKSIG));

        // Bitcoin address tx, sender provides hash of pubkey, receiver provides signature and pubkey
        mTemplates.insert(make_pair(TX_PUBKEYHASH, CScript() << OP_DUP << OP_HASH160 << OP_PUBKEYHASH << OP_EQUALVERIFY << OP_CHECKSIG));

        // Sender provides N pubkeys, receivers provides M signatures
        mTemplates.insert(make_pair(TX_MULTISIG, CScript() << OP_SMALLINTEGER << OP_PUBKEYS << OP_SMALLINTEGER << OP_CHECKMULTISIG));
    }

    vSolutionsRet.clear();

    // Shortcut for pay-to-script-hash, which are more constrained than the other types:
    // it is always OP_HASH160 20 [20 byte hash] OP_EQUAL
    if (scriptPubKey.IsPayToScriptHash())
    {
        typeRet = TX_SCRIPTHASH;
        vector<unsigned char> hashBytes(scriptPubKey.begin()+2, scriptPubKey.begin()+22);
        vSolutionsRet.push_back(hashBytes);
        return true;
    }

    int witnessversion;
    std::vector<unsigned char> witnessprogram;
    if (scriptPubKey.IsWitnessProgram(witnessversion, witnessprogram)) {
        if (witnessversion == 0 && witnessprogram.size() == 20) {
            typeRet = TX_WITNESS_V0_KEYHASH;
            vSolutionsRet.push_back(witnessprogram);
            return true;
        }
        if (witnessversion == 0 && witnessprogram.size() == 32) {
            typeRet = TX_WITNESS_V0_SCRIPTHASH;
            vSolutionsRet.push_back(witnessprogram);
            return true;
        }
        return false;
    }

    // Provably prunable, data-carrying output
    //
    // So long as script passes the IsUnspendable() test and all but the first
    // byte passes the IsPushOnly() test we don't care what exactly is in the
    // script.
    if (scriptPubKey.size() >= 1 && scriptPubKey[0] == OP_RETURN && scriptPubKey.IsPushOnly(scriptPubKey.begin()+1)) {
        typeRet = TX_NULL_DATA;
        return true;
    }

    // Scan templates
    const CScript& script1 = scriptPubKey;
    //BOOST_FOREACH(const PAIRTYPE(txnouttype, CScript)& tplate, mTemplates)
    multimap<txnouttype, CScript>::iterator it = mTemplates.begin();
    for(; it != mTemplates.end(); it++)
    {
        multimap<txnouttype, CScript>::value_type tplate = *it;
        const CScript& script2 = tplate.second;
        vSolutionsRet.clear();

        opcodetype opcode1, opcode2;
        vector<unsigned char> vch1, vch2;

        // Compare
        CScript::const_iterator pc1 = script1.begin();
        CScript::const_iterator pc2 = script2.begin();
        while (true)
        {
            if (pc1 == script1.end() && pc2 == script2.end())
            {
                // Found a match
                typeRet = tplate.first;
                if (typeRet == TX_MULTISIG)
                {
                    // Additional checks for TX_MULTISIG:
                    unsigned char m = vSolutionsRet.front()[0];
                    unsigned char n = vSolutionsRet.back()[0];
                    if (m < 1 || n < 1 || m > n || vSolutionsRet.size()-2 != n)
                        return false;
                }
                return true;
            }
            if (!script1.GetOp(pc1, opcode1, vch1))
                break;
            if (!script2.GetOp(pc2, opcode2, vch2))
                break;

            // Template matching opcodes:
            if (opcode2 == OP_PUBKEYS)
            {
                while (vch1.size() >= 33 && vch1.size() <= 65)
                {
                    vSolutionsRet.push_back(vch1);
                    if (!script1.GetOp(pc1, opcode1, vch1))
                        break;
                }
                if (!script2.GetOp(pc2, opcode2, vch2))
                    break;
                // Normal situation is to fall through
                // to other if/else statements
            }

            if (opcode2 == OP_PUBKEY)
            {
                if (vch1.size() < 33 || vch1.size() > 65)
                    break;
                vSolutionsRet.push_back(vch1);
            }
            else if (opcode2 == OP_PUBKEYHASH)
            {
                if (vch1.size() != sizeof(uint160))
                    break;
                vSolutionsRet.push_back(vch1);
            }
            else if (opcode2 == OP_SMALLINTEGER)
            {   // Single-byte small integer pushed onto vSolutions
                if (opcode1 == OP_0 ||
                    (opcode1 >= OP_1 && opcode1 <= OP_16))
                {
                    char n = (char)CScript::DecodeOP_N(opcode1);
                    vSolutionsRet.push_back(vector<unsigned char>(1, n));
                }
                else
                    break;
            }
            else if (opcode1 != opcode2 || vch1 != vch2)
            {
                // Others must match exactly
                break;
            }
        }
    }

    vSolutionsRet.clear();
    typeRet = TX_NONSTANDARD;
    return false;
}


bool IsFinalTx(const CTransaction &tx, int nBlockHeight, int64_t nBlockTime)
{
    if (tx.nLockTime == 0)
        return true;
    if ((int64_t)tx.nLockTime < ((int64_t)tx.nLockTime < LOCKTIME_THRESHOLD ? (int64_t)nBlockHeight : nBlockTime))
        return true;

    //BOOST_FOREACH(const CTxIn& txin, tx.vin)
    std::vector<CTxIn>::const_iterator it = tx.vin.begin();
    for(; it != tx.vin.end(); it++)
    {
        CTxIn txin = *it;
        if (!(txin.nSequence == CTxIn::SEQUENCE_FINAL))
            return false;
    }
    return true;
}

bool CheckFinalTx(const CTransaction &tx, int blockh)
{
    int flags = 0;
    const int nBlockHeight = blockh + 1;
    time_t now = time(NULL);
    const int64_t nBlockTime = now;
    return IsFinalTx(tx, nBlockHeight, nBlockTime);
}

isminetype Transaction::IsMine(const CTxOut& txout)
{
    CScript scriptPubKey = txout.scriptPubKey;
    vector<valtype> vSolutions;
    txnouttype whichType;
    if (!Solver(scriptPubKey, whichType, vSolutions)) {
        return ISMINE_NO;
    }

    uint160 keyid;

    switch (whichType)
    {
    case TX_NONSTANDARD:
    case TX_NULL_DATA:
        break;
    case TX_PUBKEY:
        keyid = PubKey(vSolutions[0]).GetID();
        if (store->HaveKey(keyid))
            return ISMINE_SPENDABLE;
        break;
    case TX_PUBKEYHASH:
        keyid = uint160(vSolutions[0]);
        if (store->HaveKey(keyid))
            return ISMINE_SPENDABLE;
        break;
    }

    return ISMINE_NO;
}

Transaction::Transaction(TransactionStore* s):store(s)
{
    ;
}

CAmount Transaction::GetCredit()
{
    CAmount debit = 0;
    std::map<uint256, Transaction> txs = store->GetTransactions();
    /*for(std::vector<CTxIn>::const_iterator it = this->vin.begin(); it != this->vin.end(); it++)
    {
        CTxIn txin = *it;
        std::map<uint256, Transaction>::iterator txit = txs.find(txin.prevout.hash);
        if(txit != txs.end())
        {
            Transaction prev = txit->second;
            if(txin.prevout.n < prev.vout.size())
            {
                if(IsMine(prev.vout[txin.prevout.n]) & ISMINE_SPENDABLE)
                {
                    debit = debit + prev.vout[txin.prevout.n].nValue;
                }

            }
        }
    }*/
    for(std::vector<CTxOut>::const_iterator it = this->vout.begin(); it != this->vout.end(); it++)
    {
        CTxOut txout = *it;
        if(IsMine(txout) & ISMINE_SPENDABLE)
        {
            debit = debit + txout.nValue;
        }
    }
    return debit;
}

CAmount Transaction::GetDebit()
{
    CAmount credit = 0;
    for(std::vector<CTxOut>::const_iterator it = this->vout.begin(); it != this->vout.end(); it++)
    {
        CTxOut txout = *it;
        credit = credit + txout.nValue;
        if(IsMine(txout) & ISMINE_ALL)
        {
            credit = 0;
            break;
        }
    }
    return credit;
}

CAmount Transaction::GetDebit(const isminefilter& filter)
{
    std::map<uint256, Transaction> txlist = store->GetTransactions();
    CAmount nDebit = 0;
    //BOOST_FOREACH(const CTxIn& txin, tx.vin)
    for(std::vector<CTxIn>::const_iterator it = this->vin.begin(); it != this->vin.end(); it++)
    {
        CTxIn txin = *it;

        //nDebit += GetDebit(txin, filter);
        std::map<uint256, Transaction>::iterator pit = txlist.find(txin.prevout.hash);
        if(pit != txlist.end())
        {
            Transaction prev = pit->second;
            if (txin.prevout.n < prev.vout.size())
                if (IsMine(prev.vout[txin.prevout.n]) & filter)
                    nDebit += prev.vout[txin.prevout.n].nValue;
        }
    }
    return nDebit;
}

bool Transaction::IsTrusted()
{
    // Quick answer in most cases
    if (!CheckFinalTx(*this, store->BlocksCount()))
        return false;
    int nDepth = store->BlocksCount() - this->nLockTime;
    if (nDepth >= 1)
        return true;
    if (nDepth < 0)
        return false;
    //if (!bSpendZeroConfChange || !IsFromMe(ISMINE_ALL)) // using wtx's cached debit
    //    return false;
    if( ! (GetDebit(ISMINE_ALL) > 0) )
            false;

    // Don't trust unconfirmed transactions from us unless they are in the mempool.
    //if (!InMempool())
    //    return false;

    // Trusted if all inputs are from us and are in the mempool:
    for(std::vector<CTxIn>::const_iterator it = this->vin.begin(); it != this->vin.end(); it++)
    {
        CTxIn txin = *it;
        std::map<uint256, Transaction> txlist = store->GetTransactions();
        std::map<uint256, Transaction>::iterator pit = txlist.find(txin.prevout.hash);
        if(pit == txlist.end())
            return false;
        Transaction parent = pit->second;
        const CTxOut& parentOut = parent.vout[txin.prevout.n];
        if (IsMine(parentOut) != ISMINE_SPENDABLE)
            return false;
    }
    return true;
}

int Transaction::GetDepthInMainChain() const
{
    if(!store->BlocksCount()) return 0;
    return store->BlocksCount() - this->nLockTime + 1;
    //return 0;
}

int Transaction::GetBlocksToMaturity() const
{
    if (!IsCoinBase())
        return 0;
    return max(0, (COINBASE_MATURITY+1) - GetDepthInMainChain());
}

/**
 * Outpoint is spent if any non-conflicted transaction
 * spends it:
 */
bool Transaction::IsSpent(const uint256& hash, unsigned int n) const
{
    std::map<uint256, Transaction> txlist = GetTransactions();
    std::map<uint256, Transaction>::iterator it = txlist.begin();
    for(; it != txlist.end(); it++)
    {
        std::vector<CTxIn>::iterator itin = it->second.vin.begin();
        for(; itin != it->second.vin.end(); itin++)
        {
            if(itin->prevout.hash == hash && itin->prevout.n == n)
                return true;
        }
    }
    return false;
}

TransactionStore::TransactionStore() : ctx(0), balance(0), blocks_count(0)
{

}

TransactionStore::TransactionStore(Context* c, IAddressKeyFactory* f) : ctx(c), factory(f), balance(0), blocks_count(0)
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

bool TransactionStore::HaveKey(uint160 key)
{
    QList<IAddressKey*> keyslist = factory->keys();
    for(QList<IAddressKey*>::iterator it = keyslist.begin(); it != keyslist.end(); it++)
    {
        PubKey pk = (*it)->pubKey();
        uint160 id = pk.GetID();
        if(key == id)
            return true;
    }
    return false;
}



void TransactionStore::AvailableCoins(std::vector<COutput> & vCoins, bool fOnlyConfirmed)
{
    vCoins.clear();
    std::map<uint256, Transaction>::iterator it = txlist.begin();
    for(; it != txlist.end(); it++)
    {
        const uint256& txid = it->first;
        Transaction* pcoin = &(*it).second;;

        if (!CheckFinalTx(*pcoin, (int)blocks_count))
            continue;

        if (fOnlyConfirmed && !pcoin->IsTrusted())
            continue;

        if(pcoin->IsCoinBase() && pcoin->GetBlocksToMaturity() > 0)
            continue;

        int nDepth = pcoin->GetDepthInMainChain();
        if (nDepth < 0)
            continue;
        for (unsigned int i = 0; i < pcoin->vout.size(); i++) 
        {
            isminetype mine = IsMine(pcoin->vout[i]);
            if (!(IsSpent(txid, i)) && mine != ISMINE_NO &&
                pcoin->vout[i].nValue > 0)
            {
                vCoins.push_back(COutput(pcoin, i, nDepth, (mine & ISMINE_SPENDABLE) != ISMINE_NO, 
                    (mine & ISMINE_SPENDABLE) != ISMINE_NO));
            }
        }
    }
}

bool TransactionStore::SelectCoins(const vector<COutput>& vAvailableCoins, const CAmount& nTargetValue, set<pair<const Transaction*,unsigned int> >& setCoinsRet, CAmount& nValueRet)
{
    vector<COutput> vCoins(vAvailableCoins);

    //BOOST_FOREACH(const COutput& out, vCoins)
    for(vector<COutput>::iterator it = vCoins.begin(); it != vCoins.end(); it++)
    {
        COutput out = *it;
        if (!out.fSpendable)
             continue;
        nValueRet += out.tx->vout[out.i].nValue;
        setCoinsRet.insert(make_pair(out.tx, out.i));
    }
    return (nValueRet >= nTargetValue);
}

bool TransactionStore::CreateSendTx(Transaction& tx, int amountVal, int feeVal, std::string blob, bool subsractFee, 
    CryptoAddress &from_address, CryptoAddress &to_address, int& nChangePosInOut, std::string& strFailReason)
{
    int nChangePosRequest = nChangePosInOut;
    
    if(blocks_count == 0) return false;
    CAmount nAmount = amountVal;
    //tx.vout.push_back(txout);

    CAmount nValue = 0;
    nChangePosRequest = -1;
    unsigned int nSubtractFeeFromAmount = 0;

    if (nValue < 0 || nAmount < 0)
    {
        strFailReason = "Transaction amounts must be positive";
        return false;
    }
    nValue += nAmount;

    if (subsractFee)
        nSubtractFeeFromAmount++;

    bool fTimeReceivedIsTxTime = true;
    CMutableTransaction txNew;
    txNew.nLockTime = BlocksCount();

    std::vector<COutput> vAvailableCoins;
    AvailableCoins(vAvailableCoins, true);

    //transaction fee
    CAmount nFeeRet = 0;

    // Start with no fee and loop until there is enough fee
    while (true)
    {
        nChangePosInOut = nChangePosRequest;
        txNew.vin.clear();
        txNew.vout.clear();
        txNew.wit.SetNull();
        bool fFirst = true;

        CAmount nValueToSelect = nValue;
        if (nSubtractFeeFromAmount == 0)
            nValueToSelect += nFeeRet;
        double dPriority = 0;

        CTxOut txout(nAmount, to_address.GetScript(), blob);
        if(subsractFee)
        {
            txout.nValue -= nFeeRet;
        }

        if(txout.IsDust(::minRelayTxFee))
        {
            if(subsractFee && nFeeRet > 0)
            {
                if (txout.nValue < 0)
                    strFailReason = "The transaction amount is too small to pay the fee";
                else
                    strFailReason = "The transaction amount is too small to send after the fee has been deducted";
            }
            else
                strFailReason = "Transaction amount too small";
            return false;
        }
        txNew.vout.push_back(txout);

        set<pair<const Transaction*,unsigned int> > setCoins;
        CAmount nValueIn = 0;
        if (!SelectCoins(vAvailableCoins, nValueToSelect, setCoins, nValueIn))
        {
            strFailReason = "Insufficient funds";
            return false;
        }

        set<pair<const Transaction*,unsigned int> >::iterator cit = setCoins.begin();
        for(; cit != setCoins.end(); cit++)
        {
            std::pair<const Transaction*, unsigned int> pcoin = *cit;
            CAmount nCredit = pcoin.first->vout[pcoin.second].nValue;
            //The coin age after the next block (depth+1) is used instead of the current,
            //reflecting an assumption the user would accept a bit more delay for
            //a chance at a free transaction.
            //But mempool inputs might still be in the mempool, so their age stays 0
            int age = pcoin.first->GetDepthInMainChain();
            if(age < 0)
            {
                strFailReason = "Age fail";
                return false;
            }
            if (age != 0)
                age += 1;
            dPriority += (double)nCredit * age;
        }
    }

    //CAmount fee = 7500;
    //std::map<uint256, CTransaction>::iterator it = txlist.begin();
    //for(; it != txlist.begin(); it++)
    //{
    //    ;
    //}
    return true;
}
