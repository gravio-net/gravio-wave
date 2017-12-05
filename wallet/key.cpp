#include "key.h"
#include <stdio.h>
#include <string.h>
#include "secp256k1/include/secp256k1.h"
#include "secp256k1/include/secp256k1_ecdh.h"
#include <openssl/err.h>
#include <openssl/rand.h>
#include "hash.h"
#include "crypto/sha512.h"
#include "crypto/sha256.h"
#include "support/cleanse.h"

int64_t GetPerformanceCounter()
{
    int64_t nCounter = 0;
    timeval t;
    gettimeofday(&t, NULL);
    nCounter = (int64_t)(t.tv_sec * 1000000 + t.tv_usec);
    return nCounter;
}


uint256 PubKey::GetHash()
{
    return Hash(vch, vch + size());
}

Key::Key()
{}

Key::Key(Context* c)
{
    ctx = c;
    memset(vch, 0, 32);
    fValid = false;
}


int Key::GetOSRand(unsigned char *ent32)
{
    FILE* f = fopen("/dev/urandom", "rb");
    if (f == 0) {
        return 1;
    }
    int have = 0;
    do {
        size_t n = fread(ent32 + have, sizeof(unsigned char), 32 - have, f);
        if (n <= 0 || n + have > 32) {
            return 1;
        }
        have += n;
    } while (have < 32);
    fclose(f);
    return 0;
}

int Key::GetStrongRandBytes(unsigned char* out, int num)
{
    //assert(num <= 32);
    CSHA512 hasher;
    unsigned char buf[64];

    // First source: OpenSSL's RNG
    int64_t counter = GetPerformanceCounter();
    RAND_add(&counter, sizeof(counter), 1.5);
    if (RAND_bytes(buf, 32) != 1) {
        return 1;
    }
    hasher.Write(buf, 32);
    GetOSRand(buf);
    hasher.Write(buf, 32);
    hasher.Finalize(buf);
    memcpy(out, buf, num);
    memory_cleanse(buf, 64);

    return 0;
}

bool Key::Check(const unsigned char *vch) {
    if(ctx)
        return secp256k1_ec_seckey_verify(ctx->ContextSign(), vch);
    return false;
}

void Key::MakeNew()
{
    do
    {
        if(GetStrongRandBytes(vch, sizeof(vch)))
            break;
    } while (!Check(vch));
    fValid = true;
}

PubKey Key::GetPubKey()
{
    secp256k1_pubkey pubkey;
    size_t clen = 65;
    PubKey result;
    int ret = secp256k1_ec_pubkey_create(ctx->ContextSign(), &pubkey, begin());
    if(!ret) return PubKey();
    secp256k1_ec_pubkey_serialize(ctx->ContextSign(), (unsigned char*)result.begin(), &clen, &pubkey, SECP256K1_EC_UNCOMPRESSED);
    return result;
}

bool Key::GetECDHSecret(const PubKey& pubkey, std::vector<unsigned char>& result) const
{
    if (!fValid)
        return false;
    result.resize(32, 0);
    secp256k1_pubkey pubkey_val;
    if (!secp256k1_ec_pubkey_parse(ctx->ContextSign(), &pubkey_val, &pubkey[0], pubkey.size())) {
        return false;
    }
    int ret = secp256k1_ecdh(ctx->ContextSign(), (unsigned char*)&result[0], &pubkey_val, begin());
    if(!ret) return false;
    return true;
}

KeyStore::KeyStore()
{
    ;
}

void KeyStore::SetKey(Key& k)
{
    key = k;
}

void KeyStore::AddPubKey(PubKey& k)
{
    pubkeys.push_back(k);
}
