#ifndef CONTEXT_H
#define CONTEXT_H
#include "secp256k1/include/secp256k1.h"
#include <vector>
#include "uint256.h"
#include "crypto/sha256.h"

#include "../currency.h"

#define MESSAGE_START_SIZE 4

using namespace gravio::wave;

class Context
{
public:
    typedef unsigned char MessageStartChars[MESSAGE_START_SIZE];

    enum Base58Type
    {
        PUBKEY_ADDRESS,
        SCRIPT_ADDRESS,
        SCRIPT_ADDRESS2,
        SECRET_KEY,
        EXT_PUBLIC_KEY,
        EXT_SECRET_KEY,

        MAX_BASE58_TYPES
    };
    Context(const Currency::Type&);
    void CreateSign();
    secp256k1_context* ContextSign() { return secp256k1_context_sign; }
    const std::vector<unsigned char>& Base58Prefix(Base58Type type) const { return base58Prefixes[type]; }
    std::string TransactionsListUrl() const { return tx_list_url; }
    std::string TransactionUrl() const { return tx_url; }
    Currency::Type getType() { return type; }

private:
    Currency::Type type;

    secp256k1_context* secp256k1_context_sign = NULL;

    int nDefaultPort;
    std::vector<unsigned char> base58Prefixes[MAX_BASE58_TYPES];
    MessageStartChars pchMessageStart;

    std::string tx_list_url;
    std::string tx_url;
};

#endif // CONTEXT_H
