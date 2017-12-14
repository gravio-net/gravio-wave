#ifndef KEY_H
#define KEY_H
#include "uint256.h"
#include "crypto/sha256.h"
#include "context.h"
#include "cryptoaddress.h"
#include <list>
#include "hash.h"

namespace gravio {
namespace wave {
namespace backend {


class PubKey
{
public:
    PubKey(){}

    unsigned int size() const { return GetLen(vch[0]); }
    const unsigned char* begin() const { return vch; }
    const unsigned char* end() const { return vch + size(); }
    const unsigned char& operator[](unsigned int pos) const { return vch[pos]; }
    bool IsValid() const
    {
        return size() > 0;
    }

    //! Get the 256-bit hash of this public key.
    uint256 GetHash();
    void Invalidate()
    {
        vch[0] = 0xFF;
    }
    template <typename T>
    void Set(const T pbegin, const T pend)
    {
        int len = pend == pbegin ? 0 : GetLen(pbegin[0]);
        if (len && len == (pend - pbegin))
            memcpy(vch, (unsigned char*)&pbegin[0], len);
        else
            Invalidate();
    }
    //! Get the KeyID of this public key (hash of its serialization)
    uint160 GetID() const
    {
        return Hash160(vch, vch + size());
    }

private:
    unsigned int static GetLen(unsigned char chHeader)
    {
        if (chHeader == 2 || chHeader == 3)
            return 33;
        if (chHeader == 4 || chHeader == 6 || chHeader == 7)
            return 65;
        return 0;
    }
    unsigned char vch[65];
};

class Key
{
public:
    Key();
    Key(Context* c);
    const unsigned char* begin() const { return vch; }
    const unsigned char* end() const { return vch + size(); }
    unsigned int size() const { return (fValid ? 32 : 0); }
    const unsigned char& operator[](unsigned int pos) const { return vch[pos]; }
    void MakeNew();
    PubKey GetPubKey();
    bool GetECDHSecret(const PubKey& pubkey, std::vector<unsigned char>& result) const;
    template <typename T> void Set(const T pbegin, const T pend)
    {
        if (pend - pbegin != 32) {
            fValid = false;
            return;
        }
        if (Check(&pbegin[0])) {
            memcpy(vch, (unsigned char*)&pbegin[0], 32);
            fValid = true;
        } else {
            fValid = false;
        }
    }

private:
    int GetOSRand(unsigned char *ent32);
    int GetStrongRandBytes(unsigned char* out, int num);
    bool Check(const unsigned char *vch);

    unsigned char vch[32];
    bool fValid;
    Context *ctx = NULL;
};

class KeyStore
{
public:
    KeyStore();
    void SetKey(Key& k);
    void AddPubKey(PubKey& k);
private:
    Key key;
    std::list<PubKey> pubkeys;
};

} //backend
} // wave
} // gravio

#endif // KEY_H
