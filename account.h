#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>
#include <QList>
#include <QAbstractListModel>
#include <QQuickItem>
#include <QMap>

#include "currency.h"
#include "json.h"

namespace gravio {
namespace wave {

// forward declaration
class Account;

/**
 * @brief The AccountAddress class
 * Represents account address information
 */
class AccountAddress : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(Currency::Type addressType READ addressType WRITE setAddressType NOTIFY addressTypeChanged)
    Q_PROPERTY(QString addressTypeStr READ addressTypeStr WRITE setAddressTypeStr NOTIFY addressTypeChanged)
    Q_PROPERTY(QString pubKey READ pubKey WRITE setPubKey NOTIFY pubKeyChanged)
    Q_PROPERTY(bool primary READ primary WRITE setPrimary NOTIFY primaryAddressChanged)

public:
    AccountAddress(QObject *parent = 0): QObject(parent) { }
    AccountAddress(Currency::Type type, QString address, QString label, bool primary): type_(type), address_(address), label_(label), primary_(primary) {}

    void setAddress(const QString& address) { address_ = address; emit addressChanged(); }
    QString address() const { return address_; }

    void setLabel(const QString& label) { label_ = label; emit labelChanged(); }
    QString label() const { return label_; }

    void setAddressType(Currency::Type type) { type_ = type; emit addressTypeChanged(); }
    Currency::Type addressType() const { return type_; }

    void setAddressTypeStr(const QString& type) { type_ = Currency::type(type.toStdString()); emit addressTypeChanged(); }
    QString addressTypeStr() const { return QString(Currency::name(type_).c_str()); }

    void setPubKey(const QString& pubKey) { pubKey_ = pubKey; emit pubKeyChanged(); }
    QString pubKey() const { return pubKey_; }

    void setPrimary(bool primary) { primary_ = primary; emit primaryAddressChanged(); }
    bool primary() const { return primary_; }

    void fromJSON(json::Value&);
    void toJSON(json::Value&);

signals:
    void addressChanged();
    void labelChanged();
    void addressTypeChanged();
    void pubKeyChanged();
    void primaryAddressChanged();

private:
    Currency::Type type_;
    QString address_;
    QString label_;
    QString pubKey_;
    bool primary_;
};

/**
 * @brief The ContactAddresses class
 * List of all contact addresses
 */
class AccountAddresses: public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QList<AccountAddress*> list READ getList NOTIFY listChanged)

public:
    enum AccountAddressRoles
    {
        AddressTypeRole = Qt::UserRole + 1,
        AddressRole,
        LabelRole,
        PubKeyRole,
        PrimaryRole
    };

    AccountAddresses(QObject *parent = 0);
    AccountAddresses(Account* contact, QObject *parent = 0);
    ~AccountAddresses();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    AccountAddress* getAddress(int idx);
    QList<AccountAddress*> getList() { return addresses_; }

    Q_INVOKABLE QVariant get(int idx) { return QVariant::fromValue(addresses_.at(idx)); }

    void fromJSON(json::Value&);
    void toJSON(json::Value&);

    void beginUpdate() { beginResetModel(); }
    void endUpdate() { endResetModel(); }
    void clear();

    Q_INVOKABLE QString addAddress(QString addressType, QString address, QString label, bool primary);
    Q_INVOKABLE QString removeAddress(int idx);
    Q_INVOKABLE QString updateModel();
    Q_INVOKABLE QString refetchModel();

    bool contains(const QString&);

signals:
    void listChanged();

private:
    QList<AccountAddress*> addresses_;
    Account* account_;
};

/**
 * @brief The Account class
 * Account information: attributes, addresses & etc.
 */
class AccountDb;
class Account : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString fullName READ fullName WRITE setFullName NOTIFY fullNameChanged)
    Q_PROPERTY(QString avatar READ avatar WRITE setAvatar NOTIFY avatarChanged)
    Q_PROPERTY(bool customAvatar READ customAvatar WRITE setCustomAvatar NOTIFY customAvatarChanged)
    Q_PROPERTY(QVariant addresses READ addresses NOTIFY addressesChanged)
    Q_PROPERTY(QString avatarSourcePath READ avatarSourcePath)

public:
    Account(QObject *parent = 0);
    ~Account();

    Q_INVOKABLE QString update();

    void fromJSON(json::Value&);
    void toJSON(json::Value&);

    void setName(const QString& name) { name_ = name; emit nameChanged(); }
    QString name() const { return name_; }

    void setFullName(const QString& fullName) { fullName_ = fullName; emit fullNameChanged(); }
    QString fullName() const { return fullName_; }

    void setAvatar(const QString& avatar) { avatar_ = avatar; emit avatarChanged(); }
    QString avatar() const { return avatar_; }

    void setCustomAvatar(bool customAvatar) { customAvatar_ = customAvatar; emit customAvatarChanged(); }
    bool customAvatar() const { return customAvatar_; }

    QVariant addresses() { return QVariant::fromValue(addresses_); }
    AccountAddresses* getAddresses() { return addresses_; }

    Currency::Type primaryAddressType() const;

    Q_INVOKABLE QString primaryAddressTypeStr() const;
    Q_INVOKABLE QString primaryAddress() const;
    Q_INVOKABLE QString avatarSource() const;
    Q_INVOKABLE void revertChanges();
    Q_INVOKABLE QString avatarSourceFolder() const;
    Q_INVOKABLE void copyAvatar(QString);

    QString avatarSourcePath() const { return avatarSource(); }

    void refillAddresses();

    Q_INVOKABLE void open(QString);

signals:
    void nameChanged();
    void fullNameChanged();
    void avatarChanged();
    void addressesChanged();
    void customAvatarChanged();

private:
    QString name_;
    QString fullName_;
    QString avatar_;
    bool customAvatar_;
    AccountAddresses* addresses_;
    QMap<Currency::Type, QString> privateKeys_;
    json::Document accountInfoBacked_;
    AccountDb* db_;
};

} // wave
} // gravio

#endif // ACCOUNT_H
