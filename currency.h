#ifndef CURRENCY_H
#define CURRENCY_H

#include <string>

namespace gravio {
namespace wave {

class Currency
{
public:
    enum Type
    {
        Unknown,
        GIO,
        BTC,
        LTC,
        DOGE,
        ETH
    };

    static std::string name(Currency::Type type)
    {
        switch(type)
        {
            case Unknown: return std::string("Unknown");
            case GIO: return std::string("GIO");
            case BTC: return std::string("BTC");
            case LTC: return std::string("LTC");
            case DOGE: return std::string("DOGE");
            case ETH: return std::string("ETH");
        }

        return std::string("(?)");
    }

    static Currency::Type type(const std::string& name)
    {
        if (name == std::string("GIO")) return GIO;
        else if (name == std::string("BTC")) return BTC;
        else if (name == std::string("LTC")) return LTC;
        else if (name == std::string("DOGE")) return DOGE;
        else if (name == std::string("ETH")) return ETH;

        return Unknown;
    }
};

} // wave
} // gravio

#endif // CURRENCY_H
