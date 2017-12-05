#ifndef CRYPTOADDRESS_H
#define CRYPTOADDRESS_H

#include <vector>
#include <string>
#include "hash.h"
#include "context.h"
#include "script/script.h"
#include "context.h"

/**
 * Base class for all base58-encoded data
 */
class CBase58Data
{
protected:
    //! the version byte(s)
    std::vector<unsigned char> vchVersion;

    //! the actually encoded data
    typedef std::vector<unsigned char> vector_uchar;
    vector_uchar vchData;

    CBase58Data();
    void SetData(const std::vector<unsigned char> &vchVersionIn, const void* pdata, size_t nSize);
    void SetData(const std::vector<unsigned char> &vchVersionIn, const unsigned char *pbegin, const unsigned char *pend);

public:
    bool SetString(const char* psz, unsigned int nVersionBytes = 1);
    bool SetString(const std::string& str);
    std::string ToString() const;
    int CompareTo(const CBase58Data& b58) const;

    bool operator==(const CBase58Data& b58) const { return CompareTo(b58) == 0; }
    bool operator<=(const CBase58Data& b58) const { return CompareTo(b58) <= 0; }
    bool operator>=(const CBase58Data& b58) const { return CompareTo(b58) >= 0; }
    bool operator< (const CBase58Data& b58) const { return CompareTo(b58) <  0; }
    bool operator> (const CBase58Data& b58) const { return CompareTo(b58) >  0; }
};

class CryptoAddress : public CBase58Data
{
public:
    CryptoAddress();
    CryptoAddress(Context* ctx, uint160& key);
    CryptoAddress(const std::string& strAddress) { SetString(strAddress); }

    bool IsValid() const;
    CScript GetScript() const;
private:
    Context* ctx;
};

#endif // CRYPTOADDRESS_H
