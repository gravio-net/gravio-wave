#include "account.h"
#include "accountdb.h"
#include "module.h"

#include <QFileInfo>

using namespace gravio::wave;

//
// AccountAddress
//
void AccountAddress::fromJSON(json::Value& root)
{
    setAddressType(Currency::type(root[L"addressType"].getAString()));
    setAddress(root[L"address"].getQString());
    setLabel(root[L"label"].getQString());
    setPubKey(root[L"pubKey"].getQString());
    setPrimary(root[L"primary"].getBool());
}

void AccountAddress::toJSON(json::Value& root)
{
    std::string lAddressType = Currency::name(addressType());
    root.addString(L"addressType", std::wstring(lAddressType.begin(), lAddressType.end()));
    root.addString(L"address", address().toStdWString());
    root.addString(L"label", label().toStdWString());
    root.addString(L"pubKey", pubKey().toStdWString());
    root.addBool(L"primary", primary());
}

//
// AccountAddresses
//
AccountAddresses::AccountAddresses(QObject *parent): QAbstractListModel(parent), account_(0)
{

}

AccountAddresses::AccountAddresses(Account* account, QObject *parent): QAbstractListModel(parent), account_(account)
{

}

AccountAddresses::~AccountAddresses()
{
    //
    // Always delete objects
    //
    qDeleteAll(addresses_);
    addresses_.clear();
}

int AccountAddresses::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return addresses_.size();
}

QVariant AccountAddresses::data(const QModelIndex &index, int role) const
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

QHash<int, QByteArray> AccountAddresses::roleNames() const
{
    QHash<int, QByteArray> lRoles = QAbstractListModel::roleNames();
    lRoles[AddressTypeRole] = "addressType";
    lRoles[AddressRole] = "address";
    lRoles[LabelRole] = "label";
    lRoles[PubKeyRole] = "pubKey";
    lRoles[PrimaryRole] = "primary";

    return lRoles;
}

AccountAddress* AccountAddresses::getAddress(int idx)
{
    if (idx < addresses_.size()) return addresses_.at(idx);
    return 0;
}

void AccountAddresses::fromJSON(json::Value& list)
{
    for(size_t lIdx = 0; lIdx < list.size(); lIdx++)
    {
        AccountAddress* lAddress = new AccountAddress();
        const json::Value& lItem = list[lIdx];
        lAddress->fromJSON(const_cast<json::Value&>(lItem));
        addresses_.push_back(lAddress);
    }
}

void AccountAddresses::toJSON(json::Value& root)
{
    for(int lIdx = 0; lIdx < addresses_.size(); lIdx++)
    {
        const json::Value& lItem = root.newArrayItem();
        addresses_.at(lIdx)->toJSON(const_cast<json::Value&>(lItem));
    }
}

QString AccountAddresses::addAddress(QString addressType, QString address, QString label, bool primary)
{
    beginResetModel();

    // TODO: check unique on label, address and reset primary if needed

    AccountAddress* lAddress = new AccountAddress(Currency::type(addressType.toStdString()), address, label, primary);
    addresses_.push_back(lAddress);

    endResetModel();

    return QString("");
}

QString AccountAddresses::updateModel()
{
    beginResetModel();
    endResetModel();

    return QString("");
}

QString AccountAddresses::refetchModel()
{
    beginResetModel();

    clear();

    //
    // Insert new objects
    //
    if (account_) account_->refillAddresses();

    endResetModel();

    return QString("");
}

void AccountAddresses::clear()
{
    //
    // Always delete objects
    //
    qDeleteAll(addresses_);
    addresses_.clear();
}

QString AccountAddresses::removeAddress(int idx)
{
    beginResetModel();

    if (idx < addresses_.size())
    {
        delete (AccountAddress*)addresses_.at(idx);
        addresses_.removeAt(idx);
    }

    endResetModel();

    return QString("");
}

bool AccountAddresses::contains(const QString& filter)
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
// Account
//
Account::Account(QObject *parent): QObject(parent)
{
    db_ = AccountDbFactory::get();
    db_->link(this);

    customAvatar_ = false;
    addresses_ = new AccountAddresses(this, parent);
    accountInfoBacked_.loadFromString(L"{}");
}

Account::~Account()
{
    delete addresses_;
}

void Account::open(QString secret)
{
    db_->open(secret);
}

QString Account::update()
{
    try
    {
        if (!db_) NULL_REFERENCE_EXCEPTION();
        db_->update();
    }
    catch(wave::Exception const& ex)
    {
        return QString(ex.message().c_str());
    }

    return QString();
}

Currency::Type Account::primaryAddressType() const
{
    for(int lIdx = 0; lIdx < addresses_->rowCount(); lIdx++)
    {
        AccountAddress* lAddress = addresses_->getAddress(lIdx);
        if (lAddress->primary()) return lAddress->addressType();
    }

    return Currency::Unknown;
}

QString Account::primaryAddressTypeStr() const
{
    return QString::fromStdString(Currency::name(primaryAddressType()));
}

QString Account::primaryAddress() const
{
    for(int lIdx = 0; lIdx < addresses_->rowCount(); lIdx++)
    {
        AccountAddress* lAddress = addresses_->getAddress(lIdx);
        if (lAddress->primary()) return lAddress->address();
    }

    return QString("");
}

void Account::refillAddresses()
{
    json::Value lList;
    if (accountInfoBacked_.find(L"addresses", lList))
    {
        addresses_->fromJSON(const_cast<json::Value&>(lList));
    }
}

void Account::revertChanges()
{
    json::Value lName;
    if (accountInfoBacked_.find(L"name", lName))
    {
        setName(accountInfoBacked_[L"name"].getQString());
        setFullName(accountInfoBacked_[L"fullName"].getQString());
        setAvatar(accountInfoBacked_[L"avatar"].getQString());
    }
}

QString Account::avatarSource() const
{
    if (!customAvatar_)
    {
        // TODO: get profile - "roaming" from config...
        QString lAvatarFile = ApplicationPath::assetUrlPath() + ApplicationPath::applicationUrlPath() + "/roaming/images/avatar/" + avatar_;
        return lAvatarFile;
    }
    else
    {
        QString lAvatarFile = ApplicationPath::assetUrlPath() + ApplicationPath::dataDirPath() + "/account/" + avatar_;
        return lAvatarFile;
    }

    return "";
}

QString Account::avatarSourceFolder() const
{
    return ApplicationPath::dataDirPath() + "/account/";
}

void Account::copyAvatar(QString fromUrl)
{
    QFile lFrom(QUrl(fromUrl).toLocalFile());
    lFrom.copy(avatarSourceFolder() + QFileInfo(fromUrl).fileName());
}

void Account::fromJSON(json::Value& root)
{
    setName(root[L"name"].getQString());
    setFullName(root[L"fullName"].getQString());
    setAvatar(root[L"avatar"].getQString());

    json::Value lCustomAvatar;
    if (root.find(L"customAvatar", lCustomAvatar)) setCustomAvatar(lCustomAvatar.getBool());

    addresses_->clear();

    const json::Value& lList = root[L"addresses"];
    addresses_->fromJSON(const_cast<json::Value&>(lList));

    accountInfoBacked_.loadFromString(L"{}");
    root.clone(accountInfoBacked_);
}

void Account::toJSON(json::Value& root)
{
    root.addString(L"name", name().toStdWString());
    root.addString(L"fullName", fullName().toStdWString());
    root.addString(L"avatar", avatar().toStdWString());
    root.addBool(L"customAvatar", customAvatar());

    const json::Value& lList = root.addArray(L"addresses");
    if (addresses_) addresses_->toJSON(const_cast<json::Value&>(lList));

    accountInfoBacked_.loadFromString(L"{}");
    root.clone(accountInfoBacked_);
}
