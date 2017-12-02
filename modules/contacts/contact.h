#ifndef CONTACT_H
#define CONTACT_H

#include <QString>
#include <QList>
#include <QAbstractListModel>
#include <QQuickItem>

#include "../../currency.h"
#include "../../json.h"

namespace gravio {
namespace wave {

// forward declaration
class Contact;

/**
 * @brief The ContactAddress class
 * Represents contact address information
 */
class ContactAddress : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(Currency::Type addressType READ addressType WRITE setAddressType NOTIFY addressTypeChanged)
    Q_PROPERTY(QString addressTypeStr READ addressTypeStr WRITE setAddressTypeStr NOTIFY addressTypeChanged)
    Q_PROPERTY(QString pubKey READ pubKey WRITE setPubKey NOTIFY pubKeyChanged)
    Q_PROPERTY(bool primary READ primary WRITE setPrimary NOTIFY primaryAddressChanged)

public:
    ContactAddress(QObject *parent = 0): QObject(parent) { }
    ContactAddress(Currency::Type type, QString address, QString label, bool primary): type_(type), address_(address), label_(label), primary_(primary) {}

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
class ContactAddresses: public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QList<ContactAddress*> list READ getList NOTIFY listChanged)

public:
    enum ContactAddressRoles
    {
        AddressTypeRole = Qt::UserRole + 1,
        AddressRole,
        LabelRole,
        PubKeyRole
    };

    ContactAddresses(QObject *parent = 0);
    ContactAddresses(Contact* contact, QObject *parent = 0);
    ~ContactAddresses();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    ContactAddress* getAddress(int idx);
    QList<ContactAddress*> getList() { return addresses_; }

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
    QList<ContactAddress*> addresses_;
    Contact* contact_;
};

/**
 * @brief The Contact class
 * Contact information: attributes, addresses & etc.
 */
class ContactsDb;
class Contact : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString fullName READ fullName WRITE setFullName NOTIFY fullNameChanged)
    Q_PROPERTY(QString avatar READ avatar WRITE setAvatar NOTIFY avatarChanged)
    Q_PROPERTY(bool customAvatar READ customAvatar WRITE setCustomAvatar NOTIFY customAvatarChanged)
    Q_PROPERTY(QVariant addresses READ addresses NOTIFY addressesChanged)
    Q_PROPERTY(QString avatarSourcePath READ avatarSourcePath)

public:
    Contact(QObject *parent = 0);
    ~Contact();

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
    ContactAddresses* getAddresses() { return addresses_; }

    Currency::Type primaryAddressType() const;

    Q_INVOKABLE QString primaryAddressTypeStr() const;
    Q_INVOKABLE QString primaryAddress() const;
    Q_INVOKABLE QString avatarSource() const;
    Q_INVOKABLE void revertChanges();
    Q_INVOKABLE bool isNew() { return id_ == 0; }

    Q_INVOKABLE QString avatarSourceFolder() const;
    Q_INVOKABLE void copyAvatar(QString);

    QString avatarSourcePath() const { return avatarSource(); }

    int id() { return id_; }
    void setId(int id) { id_ = id; }

    void link(ContactsDb*);
    void refillAddresses();

signals:
    void nameChanged();
    void fullNameChanged();
    void avatarChanged();
    void addressesChanged();
    void customAvatarChanged();

private:
    int id_;
    QString name_;
    QString fullName_;
    QString avatar_;
    bool customAvatar_;
    ContactAddresses* addresses_;
    json::Document contactInfoBacked_;
    ContactsDb* db_;
};

} // wave
} // gravio

//Q_DECLARE_METATYPE(gravio::wave::Contact)

#endif // CONTACT_H
