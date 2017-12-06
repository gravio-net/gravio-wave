#include "contact.h"
#include "contactsdb.h"
#include "../../module.h"

#include <QFileInfo>

using namespace gravio::wave;

//
// ContactAddress
//
void ContactAddress::fromJSON(json::Value& root)
{
    setAddressType(Currency::type(root[L"addressType"].getAString()));
    setAddress(root[L"address"].getQString());
    setLabel(root[L"label"].getQString());
    setPubKey(root[L"pubKey"].getQString());
    setPrimary(root[L"primary"].getBool());
}

void ContactAddress::toJSON(json::Value& root)
{
    std::string lAddressType = Currency::name(addressType());
    root.addString(L"addressType", std::wstring(lAddressType.begin(), lAddressType.end()));
    root.addString(L"address", address().toStdWString());
    root.addString(L"label", label().toStdWString());
    root.addString(L"pubKey", pubKey().toStdWString());
    root.addBool(L"primary", primary());
}

//
// ContactAddresses
//
ContactAddresses::ContactAddresses(QObject *parent): QAbstractListModel(parent), contact_(0)
{

}

ContactAddresses::ContactAddresses(Contact* contact, QObject *parent): QAbstractListModel(parent), contact_(contact)
{

}

ContactAddresses::~ContactAddresses()
{
    //
    // Always delete objects
    //
    qDeleteAll(addresses_);
    addresses_.clear();
}

int ContactAddresses::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return addresses_.size();
}

QVariant ContactAddresses::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    switch (role)
    {
    case AddressTypeRole:
        return addresses_.at(index.row())->addressTypeStr();
    case AddressRole:
        return addresses_.at(index.row())->address();
    case LabelRole:
        return addresses_.at(index.row())->label();
    case PubKeyRole:
        return addresses_.at(index.row())->pubKey();
    case PrimaryRole:
        return addresses_.at(index.row())->primary();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ContactAddresses::roleNames() const
{
    QHash<int, QByteArray> lRoles = QAbstractListModel::roleNames();
    lRoles[AddressTypeRole] = "addressType";
    lRoles[AddressRole] = "address";
    lRoles[LabelRole] = "label";
    lRoles[PubKeyRole] = "pubKey";
    lRoles[PrimaryRole] = "primary";

    return lRoles;
}

ContactAddress* ContactAddresses::getAddress(int idx)
{
    if (idx < addresses_.size()) return addresses_.at(idx);
    return 0;
}

void ContactAddresses::fromJSON(json::Value& list)
{
    for(size_t lIdx = 0; lIdx < list.size(); lIdx++)
    {
        ContactAddress* lAddress = new ContactAddress();
        const json::Value& lItem = list[lIdx];
        lAddress->fromJSON(const_cast<json::Value&>(lItem));
        addresses_.push_back(lAddress);
    }
}

void ContactAddresses::toJSON(json::Value& root)
{
    for(int lIdx = 0; lIdx < addresses_.size(); lIdx++)
    {
        json::Value lItem = root.newArrayItem();
        addresses_.at(lIdx)->toJSON(lItem);
    }
}

QString ContactAddresses::addAddress(QString addressType, QString address, QString label, bool primary)
{
    beginResetModel();

    // TODO: check unique on label, address and reset primary if needed

    ContactAddress* lAddress = new ContactAddress(Currency::type(addressType.toStdString()), address, label, primary);
    addresses_.push_back(lAddress);

    endResetModel();

    return QString("");
}

QString ContactAddresses::updateModel()
{
    beginResetModel();
    endResetModel();

    return QString("");
}

QString ContactAddresses::refetchModel()
{
    beginResetModel();

    clear();

    //
    // Insert new objects
    //
    if (contact_) contact_->refillAddresses();

    endResetModel();

    return QString("");
}

void ContactAddresses::clear()
{
    //
    // Always delete objects
    //
    qDeleteAll(addresses_);
    addresses_.clear();
}

QString ContactAddresses::removeAddress(int idx)
{
    beginResetModel();

    if (idx < addresses_.size())
    {
        delete (ContactAddress*)addresses_.at(idx);
        addresses_.removeAt(idx);
    }

    endResetModel();

    return QString("");
}

bool ContactAddresses::contains(const QString& filter)
{
    for(int lIdx = 0; lIdx < addresses_.size(); lIdx++)
    {
        if (addresses_[lIdx]->address().contains(filter, Qt::CaseSensitivity::CaseInsensitive) ||
                addresses_[lIdx]->addressTypeStr().contains(filter, Qt::CaseSensitivity::CaseInsensitive))
            return true;
    }

    return false;
}

//
// Contact
//
Contact::Contact(QObject *parent): QObject(parent)
{
    id_ = 0;
    db_ = 0;
    customAvatar_ = false;
    addresses_ = new ContactAddresses(this, parent);
    contactInfoBacked_.loadFromString(L"{}");
}

Contact::~Contact()
{
    delete addresses_;
}

void Contact::link(ContactsDb* db)
{
    db_ = db;
}

QString Contact::update()
{
    try
    {
        if (!db_) NULL_REFERENCE_EXCEPTION();
        db_->update(this);
    }
    catch(wave::Exception const& ex)
    {
        return QString(ex.message().c_str());
    }

    return QString();
}

Currency::Type Contact::primaryAddressType() const
{
    for(int lIdx = 0; lIdx < addresses_->rowCount(); lIdx++)
    {
        ContactAddress* lAddress = addresses_->getAddress(lIdx);
        if (lAddress->primary()) return lAddress->addressType();
    }

    return Currency::Unknown;
}

QString Contact::primaryAddressTypeStr() const
{
    return QString::fromStdString(Currency::name(primaryAddressType()));
}

QString Contact::primaryAddress() const
{
    for(int lIdx = 0; lIdx < addresses_->rowCount(); lIdx++)
    {
        ContactAddress* lAddress = addresses_->getAddress(lIdx);
        if (lAddress->primary()) return lAddress->address();
    }

    return QString("");
}

void Contact::refillAddresses()
{
    const json::Value& lList = contactInfoBacked_[L"addresses"];
    addresses_->fromJSON(const_cast<json::Value&>(lList));
}

void Contact::revertChanges()
{
    if (!id_) return; // new

    setName(contactInfoBacked_[L"name"].getQString());
    setFullName(contactInfoBacked_[L"fullName"].getQString());
    setAvatar(contactInfoBacked_[L"avatar"].getQString());
}

QString Contact::avatarSource() const
{
    if (!customAvatar_)
    {
        // TODO: get profile - "roaming" from config...
        QString lAvatarFile = ApplicationPath::assetUrlPath() + ApplicationPath::applicationUrlPath() + "/roaming/images/avatar/" + avatar_;
        return lAvatarFile;
    }
    else
    {
        QString lAvatarFile = ApplicationPath::assetUrlPath() + ApplicationPath::dataDirPath() + "/contacts/" + avatar_;
        return lAvatarFile;
    }

    return "";
}

QString Contact::avatarSourceFolder() const
{
    return ApplicationPath::dataDirPath() + "/contacts/";
}

void Contact::copyAvatar(QString fromUrl)
{
    QFile lFrom(QUrl(fromUrl).toLocalFile());
    lFrom.copy(avatarSourceFolder() + QFileInfo(fromUrl).fileName());
}

void Contact::fromJSON(json::Value& root)
{
    setId(root[L"id"].getInt());
    setName(root[L"name"].getQString());
    setFullName(root[L"fullName"].getQString());
    setAvatar(root[L"avatar"].getQString());

    json::Value lCustomAvatar;
    if (root.find(L"customAvatar", lCustomAvatar)) setCustomAvatar(lCustomAvatar.getBool());

    addresses_->clear();

    const json::Value& lList = root[L"addresses"];
    addresses_->fromJSON(const_cast<json::Value&>(lList));

    contactInfoBacked_.loadFromString(L"{}");
    root.clone(contactInfoBacked_);
}

void Contact::toJSON(json::Value& root)
{
    root.addInt(L"id", id());
    root.addString(L"name", name().toStdWString());
    root.addString(L"fullName", fullName().toStdWString());
    root.addString(L"avatar", avatar().toStdWString());
    root.addBool(L"customAvatar", customAvatar());

    const json::Value& lList = root.addArray(L"addresses");
    if (addresses_) addresses_->toJSON(const_cast<json::Value&>(lList));

    contactInfoBacked_.loadFromString(L"{}");
    root.clone(contactInfoBacked_);
}
