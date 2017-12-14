#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>
#include <QList>
#include <QAbstractListModel>
#include <QQuickItem>
#include <QMap>

#include "currency.h"
#include "json.h"
#include "wallet/walletstore.h"

namespace gravio {
namespace wave {

using namespace backend;

/**
 * @brief The AddressKey facade class
 * Holds currency address (key/pubKey)
 */
class AddressKey
{
public:
    AddressKey(Context* context) : context_(context), type_(context->getType()), key_(context)
    {
        primary_ = false; label_ = "";
    }
    ~AddressKey()
    {
    }

    void fromJSON(json::Value&);
    void toJSON(json::Value&);

    Key& key() { return key_; }
    PubKey& pubKey() { return pubKey_; }

    Currency::Type type() { return type_; }
    Context* context() { return context_; }

    bool primary() { return primary_; }
    void setPrimary(bool primary) { primary_ = primary; }

    QString label() { return label_; }
    void setLabel(QString label) { label_ = label; }

private:
    Context* context_;
    Currency::Type type_;
    Key key_;
    PubKey pubKey_;
    QString label_;
    bool primary_;
};

/**
 * @brief The AddressKeyFactory facade class
 * Produces new AddressKey
 */
class AddressKeyFactory
{
public:
    AddressKeyFactory(Currency::Type type) : type_(type), ctx_(type) {}
    ~AddressKeyFactory();

    AddressKey* newKey();

    QList<AddressKey*> keys() { return keys_; }
    Currency::Type type() { return type_; }

    void fromJSON(json::Value&);
    void toJSON(json::Value&);

    void clear();

private:
    Currency::Type type_;
    Context ctx_;
    QList<AddressKey*> keys_;
};

// forward declaration
class Account;

/**
 * @brief The AccountAddress class
 * Represents account address information
 */
class AccountAddress : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString address READ address)
    Q_PROPERTY(QString addressType READ addressType)
    Q_PROPERTY(QString originalAddressType READ originalAddressType)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(bool primary READ primary WRITE setPrimary NOTIFY primaryAddressChanged)

public:
    AccountAddress(QObject *parent = 0): QObject(parent) { key_ = 0; }
    AccountAddress(Currency::Type type, AddressKey* key, bool hideAddressType): key_(key), type_(type) { hideAddressType_ = hideAddressType; }

    QString address()
    {
        if (!address_.length())
        {
            uint160 lId = key_->pubKey().GetID();
            CryptoAddress lAddress(key_->context(), lId);

            std::string lAddressStr = lAddress.ToString();
            address_ = QString::fromStdString(lAddressStr);
        }

        return address_;
    }

    void setLabel(const QString& label) { key_->setLabel(label); emit labelChanged(); }
    QString label() const { return key_->label(); }

    QString addressType() const { return hideAddressType_ ? "" : QString(Currency::name(type_).c_str()); }
    QString originalAddressType() const { return QString(Currency::name(type_).c_str()); }

    void setPrimary(bool primary) { key_->setPrimary(primary); emit primaryAddressChanged(); }
    bool primary() const { return key_->primary(); }

signals:
    void labelChanged();
    void primaryAddressChanged();

private:
    AddressKey* key_;
    Currency::Type type_;
    bool hideAddressType_;
    QString address_;
};

/**
 * @brief The AccountAddresses class
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
        OriginalAddressTypeRole,
        AddressRole,
        LabelRole,
        PrimaryRole
    };

    AccountAddresses(QObject *parent = 0);
    AccountAddresses(Account* contact, QObject *parent = 0);
    ~AccountAddresses();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;
    QList<AccountAddress*> getList() { return addresses_; }

    Q_INVOKABLE QVariant get(int idx) { return QVariant::fromValue(addresses_.at(idx)); }

    void fromJSON(json::Value&);
    void toJSON(json::Value&);

    void beginUpdate() { beginResetModel(); }
    void endUpdate() { endResetModel(); }

    Q_INVOKABLE QString addAddress(QString addressType, QString label, bool primary);
    Q_INVOKABLE QString updateModel();
    Q_INVOKABLE QString refetchModel();

    AddressKeyFactory* getAddressFactory(Currency::Type type);

    void clear();

private:
    void makeAddresses();

signals:
    void listChanged();

private:
    QList<AccountAddress*> addresses_;
    Account* account_;
    QMap<Currency::Type, AddressKeyFactory*> factories_;
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

    Q_INVOKABLE QString avatarSource() const;
    Q_INVOKABLE void revertChanges();
    Q_INVOKABLE QString avatarSourceFolder() const;
    Q_INVOKABLE void copyAvatar(QString);

    QString avatarSourcePath() const { return avatarSource(); }

    void refillAddresses();

    Q_INVOKABLE void open(QString);

    //
    AddressKeyFactory* getAddressFactory(Currency::Type type) { return addresses_->getAddressFactory(type); }

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
    json::Document accountInfoBacked_;
    AccountDb* db_;
    Wallet w_;
};

} // wave
} // gravio

#endif // ACCOUNT_H
