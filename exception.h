#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QException>

namespace gravio {
namespace wave {

class ModuleAlreadyExistsException : public QException
{
public:
    void raise() const { throw *this; }
    ModuleAlreadyExistsException *clone() const { return new ModuleAlreadyExistsException(*this); }
};

}
}

#endif // EXCEPTION_H
