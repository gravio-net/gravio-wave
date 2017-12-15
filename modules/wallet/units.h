#ifndef UNITS_H
#define UNITS_H

#include "../../currency.h"
#include "../../iaccount.h"

#include <QAbstractListModel>
#include <QString>

namespace gravio {
namespace wave {

/** Currency unit definitions. Encapsulates parsing and formatting
   and serves as list model for drop-down selection boxes.
*/
class CurrencyUnits: public QAbstractListModel
{
    Q_OBJECT

public:
    explicit CurrencyUnits(IAddressKeyFactory *factory, QObject *parent = 0);

    enum SeparatorStyle
    {
        separatorNever,
        separatorStandard,
        separatorAlways
    };

    QList<Currency::Unit> availableUnits();
    bool valid(int unit);
    QString name(int unit) const;
    QString description(int unit) const;
    qint64 factor(int unit);
    int decimals(int unit);
    QString format(int unit, const int64_t& amount, bool plussign=false, SeparatorStyle separators=separatorStandard);
    QString formatWithUnit(int unit, const int64_t& amount, bool plussign=false, SeparatorStyle separators=separatorStandard);
    QString formatHtmlWithUnit(int unit, const int64_t& amount, bool plussign=false, SeparatorStyle separators=separatorStandard);
    bool parse(int unit, const QString &value, int64_t *val_out);
    QString getAmountColumnTitle(int unit);

    enum RoleIndex
    {
        UnitRole = Qt::UserRole
    };

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    QString removeSpaces(QString text)
    {
        text.remove(' ');
        text.remove(QChar(THIN_SP_CP));
#if (THIN_SP_CP != REAL_THIN_SP_CP)
        text.remove(QChar(REAL_THIN_SP_CP));
#endif
        return text;
    }

    int64_t maxMoney();

private:
    IAddressKeyFactory *factory_;
    QList<Currency::Unit> unitList_;
};

} // wave
} // gravio

#endif // UNITS_H
