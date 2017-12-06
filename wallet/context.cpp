#include "context.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include "secp256k1/include/secp256k1.h"
#include "secp256k1/include/secp256k1_ecdh.h"

Context::Context(const Currency::Type& t = Currency::GIO)
{
    type = t;
    switch(t)
    {
    case Currency::BTC:
        break;
    case Currency::DOGE:
        pchMessageStart[0] = 0xc0;
        pchMessageStart[1] = 0xc0;
        pchMessageStart[2] = 0xc0;
        pchMessageStart[3] = 0xc0;

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,30);  // 0x1e
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,22);  // 0x16
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,158); // 0x9e

        //base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x02)(0xfa)(0xca)(0xfd).convert_to_container<std::vector<unsigned char> >();
        //base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x02)(0xfa)(0xc3)(0x98).convert_to_container<std::vector<unsigned char> >();

        nDefaultPort = 22556;
        break;
     default:
        pchMessageStart[0] = 0xad;
        pchMessageStart[1] = 0x9f;
        pchMessageStart[2] = 0x7b;
        pchMessageStart[3] = 0xfa;

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,195);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,97);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,61);

        //base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x05)(0x25)(0xFE)(0xBC).convert_to_container<std::vector<unsigned char> >();
        //base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x05)(0x25)(0xC3)(0x9B).convert_to_container<std::vector<unsigned char> >();

        nDefaultPort = 23333;
        tx_list_url = "http://blockexplorer.gravio.net/ext/getaddress/";
        tx_url = "http://blockexplorer.gravio.net/api/getrawtransaction";
        break;
    }

    CreateSign();
}

void Context::CreateSign()
{
    secp256k1_context_sign = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
}
