#ifndef IAPPLICATION_H
#define IAPPLICATION_H

#include <QQmlApplicationEngine>

#include "iaccount.h"

namespace gravio {
namespace wave {

class IApplication
{
public:
    virtual QQmlApplicationEngine* getEngine() = 0;
    virtual IAccount* getAccount() = 0;
};

} // wave
} // gravio

#endif // IAPPLICATION_H
