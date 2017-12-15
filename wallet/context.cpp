#include "context.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include "secp256k1/include/secp256k1.h"
#include "secp256k1/include/secp256k1_ecdh.h"

using namespace gravio::wave::backend;

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
        confirmations_ = 6;
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
        confirmations_ = 6;
        tx_list_url = "http://blockexplorer.gravio.net/ext/getaddress/";
        // TODO: test?
        //tx_list_url = "http://blockexplorer.gravio.net/ext/getaddress/";
        tx_url = "http://blockexplorer.gravio.net/api/getrawtransaction";
        block_count_url = "http://blockexplorer.gravio.net/api/getblockcount";
        break;
    }

    CreateSign();
}

void Context::CreateSign()
{
    secp256k1_context_sign = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
}

QString Context::unitName(Currency::Unit unit)
{
    switch(type)
    {
    case Currency::BTC:
        switch (unit)
        {
            case Currency::Unit::COIN: return "BTC";
            case Currency::Unit::mCOIN: return "mBTC";
            case Currency::Unit::uCOIN: return QString::fromUtf8("ОјBTC");
        }
        break;
    case Currency::DOGE:
        switch (unit)
        {
            case Currency::Unit::COIN: return "DOGE";
            case Currency::Unit::mCOIN: return "mDOGE";
            case Currency::Unit::uCOIN: return QString::fromUtf8("ОјDOGE");
        }
        break;
    case Currency::GIO:
        switch (unit)
        {
            case Currency::Unit::COIN: return "GIO";
            case Currency::Unit::mCOIN: return "mGIO";
            case Currency::Unit::uCOIN: return QString::fromUtf8("ОјGIO");
        }
        break;
    case Currency::LTC:
        switch (unit)
        {
            case Currency::Unit::COIN: return "LTC";
            case Currency::Unit::mCOIN: return "mLTC";
            case Currency::Unit::uCOIN: return QString::fromUtf8("ОјLTC");
        }
        break;
    }

    return QString("?");
}

QString Context::unitDescription(Currency::Unit unit)
{
    switch(type)
    {
    case Currency::BTC:
        switch (unit)
        {
            case Currency::Unit::COIN: return QString("Bitcoins");
            case Currency::Unit::mCOIN: return QString("Milli-Bitcoins (1 / 1" THIN_SP_UTF8 "000)");
            case Currency::Unit::uCOIN: return QString("Micro-Bitcoins (1 / 1" THIN_SP_UTF8 "000" THIN_SP_UTF8 "000)");
        }
        break;
    case Currency::DOGE:
        switch (unit)
        {
        case Currency::Unit::COIN: return QString("Dogecoins");
        case Currency::Unit::mCOIN: return QString("Milli-Dogecoins (1 / 1" THIN_SP_UTF8 "000)");
        case Currency::Unit::uCOIN: return QString("Micro-Dogecoins (1 / 1" THIN_SP_UTF8 "000" THIN_SP_UTF8 "000)");
        }
        break;
    case Currency::GIO:
        switch (unit)
        {
        case Currency::Unit::COIN: return QString("Graviocoins");
        case Currency::Unit::mCOIN: return QString("Milli-Graviocoins (1 / 1" THIN_SP_UTF8 "000)");
        case Currency::Unit::uCOIN: return QString("Micro-Graviocoins (1 / 1" THIN_SP_UTF8 "000" THIN_SP_UTF8 "000)");
        }
        break;
    case Currency::LTC:
        switch (unit)
        {
        case Currency::Unit::COIN: return QString("Litecoins");
        case Currency::Unit::mCOIN: return QString("Milli-Litecoins (1 / 1" THIN_SP_UTF8 "000)");
        case Currency::Unit::uCOIN: return QString("Micro-Litecoins (1 / 1" THIN_SP_UTF8 "000" THIN_SP_UTF8 "000)");
        }
        break;
    }

    return QString("?");
}

qint64 Context::unitFactor(Currency::Unit unit)
{
    switch(type)
    {
    case Currency::BTC:
    case Currency::DOGE:
    case Currency::GIO:
    case Currency::LTC:
        switch (unit)
        {
        case Currency::Unit::COIN: return 100000000;
        case Currency::Unit::mCOIN: return 100000;
        case Currency::Unit::uCOIN: return 100;
        default: return 100000000;
        }

        break;
    }

    return 100000000;
}

int Context::unitDecimals(Currency::Unit unit)
{
    switch(type)
    {
    case Currency::BTC:
    case Currency::DOGE:
    case Currency::GIO:
    case Currency::LTC:
        switch (unit)
        {
        case Currency::Unit::COIN: return 8;
        case Currency::Unit::mCOIN: return 5;
        case Currency::Unit::uCOIN: return 2;
        default: return 0;
        }

        break;
    }

    return 0;
}

static const int64_t _COIN = 100000000;
static const int64_t _CENT = 1000000;

int64_t Context::unitMaxMoney()
{
    switch(type)
    {
    case Currency::BTC: return 84000000 * _COIN;
    case Currency::LTC: return 84000000 * _COIN;
    case Currency::DOGE: return 10000000000 * _COIN;
    case Currency::GIO: return 2000000000 * _COIN;
    }
    return 0;
}
