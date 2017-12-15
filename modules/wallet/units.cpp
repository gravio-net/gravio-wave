#include "units.h"

using namespace gravio::wave;

#include <QStringList>
#include <QQmlApplicationEngine>

CurrencyUnits::CurrencyUnits(IAddressKeyFactory *factory, QObject *parent):
        QAbstractListModel(parent),
        factory_(factory),
        unitList_(availableUnits())

{
    QQmlApplicationEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

QList<Currency::Unit> CurrencyUnits::availableUnits()
{
    QList<Currency::Unit> lUnitlist;
    lUnitlist.append(Currency::Unit::COIN);
    lUnitlist.append(Currency::Unit::mCOIN);
    lUnitlist.append(Currency::Unit::uCOIN);
    return lUnitlist;
}

bool CurrencyUnits::valid(int unit)
{
    switch(unit)
    {
    case Currency::Unit::COIN:
    case Currency::Unit::mCOIN:
    case Currency::Unit::uCOIN:
        return true;
    default:
        return false;
    }
}

QString CurrencyUnits::name(int unit) const
{
    return factory_->unitName((Currency::Unit)unit);
}

QString CurrencyUnits::description(int unit) const
{
    return factory_->unitDescription((Currency::Unit)unit);
}

qint64 CurrencyUnits::factor(int unit)
{
    return factory_->unitFactor((Currency::Unit)unit);
}

int CurrencyUnits::decimals(int unit)
{
    return factory_->unitDecimals((Currency::Unit)unit);
}

QString CurrencyUnits::format(int unit, const int64_t& nIn, bool fPlus, SeparatorStyle separators)
{
    // Note: not using straight sprintf here because we do NOT want
    // localized number formatting.
    if(!valid(unit))
        return QString(); // Refuse to format invalid unit
    qint64 n = (qint64)nIn;
    qint64 coin = factor(unit);
    int num_decimals = decimals(unit);
    qint64 n_abs = (n > 0 ? n : -n);
    qint64 quotient = n_abs / coin;
    qint64 remainder = n_abs % coin;
    QString quotient_str = QString::number(quotient);
    QString remainder_str = QString::number(remainder).rightJustified(num_decimals, '0');

    // Use SI-style thin space separators as these are locale independent and can't be
    // confused with the decimal marker.
    QChar thin_sp(THIN_SP_CP);
    int q_size = quotient_str.size();
    if (separators == separatorAlways || (separators == separatorStandard && q_size > 4))
        for (int i = 3; i < q_size; i += 3)
            quotient_str.insert(q_size - i, thin_sp);

    if (n < 0)
        quotient_str.insert(0, '-');
    else if (fPlus && n > 0)
        quotient_str.insert(0, '+');
    return quotient_str + QString(".") + remainder_str;
}


// NOTE: Using formatWithUnit in an HTML context risks wrapping
// quantities at the thousands separator. More subtly, it also results
// in a standard space rather than a thin space, due to a bug in Qt's
// XML whitespace canonicalisation
//
// Please take care to use formatHtmlWithUnit instead, when
// appropriate.

QString CurrencyUnits::formatWithUnit(int unit, const int64_t& amount, bool plussign, SeparatorStyle separators)
{
    return format(unit, amount, plussign, separators) + QString(" ") + name(unit);
}

QString CurrencyUnits::formatHtmlWithUnit(int unit, const int64_t& amount, bool plussign, SeparatorStyle separators)
{
    QString str(formatWithUnit(unit, amount, plussign, separators));
    str.replace(QChar(THIN_SP_CP), QString(THIN_SP_HTML));
    return QString("<span style='white-space: nowrap;'>%1</span>").arg(str);
}

bool CurrencyUnits::parse(int unit, const QString &value, int64_t *val_out)
{
    if(!valid(unit) || value.isEmpty())
        return false; // Refuse to parse invalid unit or empty string
    int num_decimals = decimals(unit);

    // Ignore spaces and thin spaces when parsing
    QStringList parts = removeSpaces(value).split(".");

    if(parts.size() > 2)
    {
        return false; // More than one dot
    }
    QString whole = parts[0];
    QString decimals;

    if(parts.size() > 1)
    {
        decimals = parts[1];
    }
    if(decimals.size() > num_decimals)
    {
        return false; // Exceeds max precision
    }
    bool ok = false;
    QString str = whole + decimals.leftJustified(num_decimals, '0');

    if(str.size() > 18)
    {
        return false; // Longer numbers will exceed 63 bits
    }
    int64_t retvalue(str.toLongLong(&ok));
    if(val_out)
    {
        *val_out = retvalue;
    }
    return ok;
}

QString CurrencyUnits::getAmountColumnTitle(int unit)
{
    QString amountTitle = QObject::tr("Amount");
    if (valid(unit))
    {
        amountTitle += " ("+CurrencyUnits::name(unit) + ")";
    }
    return amountTitle;
}

int CurrencyUnits::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return unitList_.size();
}

QVariant CurrencyUnits::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row >= 0 && row < unitList_.size())
    {
        Currency::Unit unit = unitList_.at(row);
        switch(role)
        {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return name(static_cast<int>(unit));
        case Qt::ToolTipRole:
            return description(static_cast<int>(unit));
        case UnitRole:
            return QVariant(static_cast<int>(unit));
        }
    }
    return QVariant();
}

int64_t CurrencyUnits::maxMoney()
{
    return factory_->unitMaxMoney();
}
