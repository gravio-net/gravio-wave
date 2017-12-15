#ifndef CURRENCY_H
#define CURRENCY_H

#include <string>

// U+2009 THIN SPACE = UTF-8 E2 80 89
#define REAL_THIN_SP_CP 0x2009
#define REAL_THIN_SP_UTF8 "\xE2\x80\x89"
#define REAL_THIN_SP_HTML "&thinsp;"

// U+200A HAIR SPACE = UTF-8 E2 80 8A
#define HAIR_SP_CP 0x200A
#define HAIR_SP_UTF8 "\xE2\x80\x8A"
#define HAIR_SP_HTML "&#8202;"

// U+2006 SIX-PER-EM SPACE = UTF-8 E2 80 86
#define SIXPEREM_SP_CP 0x2006
#define SIXPEREM_SP_UTF8 "\xE2\x80\x86"
#define SIXPEREM_SP_HTML "&#8198;"

// U+2007 FIGURE SPACE = UTF-8 E2 80 87
#define FIGURE_SP_CP 0x2007
#define FIGURE_SP_UTF8 "\xE2\x80\x87"
#define FIGURE_SP_HTML "&#8199;"

#define HTML_HACK_SP "<span style='white-space: nowrap; font-size: 6pt'> </span>"

// Define THIN_SP_* variables to be our preferred type of thin space
#define THIN_SP_CP   REAL_THIN_SP_CP
#define THIN_SP_UTF8 REAL_THIN_SP_UTF8
#define THIN_SP_HTML HTML_HACK_SP

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

    enum Unit
    {
        COIN,
        mCOIN,
        uCOIN
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
