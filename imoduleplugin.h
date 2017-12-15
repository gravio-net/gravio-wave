#ifndef IMODULEPLUGIN_H
#define IMODULEPLUGIN_H

#include "iaccount.h"

namespace gravio {
namespace wave {

class IModulePlugin
{
public:
    virtual void initializeModule(IAccount*) = 0;
};

} // wave
} // gravio

#endif // IMODULEPLUGIN_H
