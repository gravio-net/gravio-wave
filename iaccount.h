#ifndef IACCOUNT_H
#define IACCOUNT_H

#include <QList>

#include "currency.h"

namespace gravio {
namespace wave {

class IAddressKey
{};

class IAddressKeyFactory
{
public:
    virtual QList<IAddressKey*> keys() = 0;
    virtual Currency::Type type() = 0;
    virtual int confirmations() = 0;
    virtual QString unitName(Currency::Unit unit) = 0;
    virtual QString unitDescription(Currency::Unit unit) = 0;
    virtual qint64 unitFactor(Currency::Unit unit) = 0;
    virtual int unitDecimals(Currency::Unit unit) = 0;
    virtual int64_t unitMaxMoney() = 0;
};

class IAccount
{
public:
    virtual IAddressKeyFactory* getAddressFactory(Currency::Type type) = 0;
    virtual QList<Currency::Type> getAddressTypes() = 0;
};

} // wave
} // gravio

#endif // IACCOUNT_H
