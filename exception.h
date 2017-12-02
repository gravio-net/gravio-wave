#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QException>

namespace gravio {
namespace wave {

//
// common exception
//
class Exception: public QException
{
public:
    Exception(const std::string& message): message_(message) {}

    const std::string& message() const { return message_; }

private:
    std::string message_;
};

#define NULL_REFERENCE_EXCEPTION()\
    { \
        char lMsg[512] = {0}; \
        snprintf(lMsg, sizeof(lMsg)-1, "Null reference exception at %s(), %s: %d", __FUNCTION__, __FILE__, __LINE__); \
        throw Exception(std::string(lMsg)); \
    } \

//
// special cases
//
class ModuleAlreadyExistsException : public QException
{
public:
    void raise() const { throw *this; }
    ModuleAlreadyExistsException *clone() const { return new ModuleAlreadyExistsException(*this); }
};

}
}

#endif // EXCEPTION_H
