#include "account.h"
#include "accountdb.h"
#include "module.h"
#include "wallet/utilstrencodings.h"

#include <QFileInfo>

using namespace gravio::wave;

//
// AddressKey
//
void AddressKey::fromJSON(json::Value& root)
{
    std::string lKey = root[L"key"].getAString();
    std::vector<unsigned char> lHexKey = ParseHex(lKey);
    key_.Set(lHexKey.begin(), lHexKey.end());

    std::string lPubKeyStr = root[L"pubKey"].getAString();
    std::vector<unsigned char> lPubKeyHex = ParseHex(lPubKeyStr);
    pubKey_.Set(lPubKeyHex.begin(), lPubKeyHex.end());

    setLabel(root[L"label"].getQString());
    setPrimary(root[L"primary"].getBool());
}

void AddressKey::toJSON(json::Value& root)
{
    std::string lKeyHexStr = HexStr(key_.begin(), key_.end());
    root.addString(L"key", std::wstring(lKeyHexStr.begin(), lKeyHexStr.end()));

    std::string lPubKeyHexStr = HexStr(pubKey_.begin(), pubKey_.end());
    root.addString(L"pubKey", std::wstring(lPubKeyHexStr.begin(), lPubKeyHexStr.end()));
    root.addString(L"label", std::wstring(label().toStdWString()));
    root.addBool(L"primary", primary());
}

//
// AddressKeyFactory
//

AddressKeyFactory::~AddressKeyFactory()
{
    clear();
}

void AddressKeyFactory::clear()
{
    qDeleteAll(keys_);
    keys_.clear();
}

AddressKey* AddressKeyFactory::newKey()
{
    AddressKey* lKey = new AddressKey(&ctx_);
    lKey->key().MakeNew(); // make new secret

    PubKey lPubKey = lKey->key().GetPubKey(); // make new pubKey
    lKey->pubKey().Set(lPubKey.begin(), lPubKey.end());

    keys_.push_back(lKey);

    return lKey;
}

void AddressKeyFactory::fromJSON(json::Value& root)
{
    json::Value lList;
    if (root.find(L"keys", lList))
    {
        for(size_t lIdx = 0; lIdx < lList.size(); lIdx++)
        {
            AddressKey* lAddressKey = new AddressKey(&ctx_);
            json::Value lEntry = lList[lIdx];
            lAddressKey->fromJSON(lEntry);
            keys_.push_back(lAddressKey);
        }
    }
}

void AddressKeyFactory::toJSON(json::Value& root)
{
    std::string lAddressType = Currency::name(type());
    root.addString(L"type", std::wstring(lAddressType.begin(), lAddressType.end()));

    json::Value lList = root.addArray(L"keys");

    for(int lIdx = 0; lIdx < keys_.size(); lIdx++)
    {
        AddressKey* lKey = keys_[lIdx];
        json::Value lItem = lList.newArrayItem();
        lKey->toJSON(lItem);
    }
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
    clear();
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
        return addresses_.at(index.row())->addressType();
    case OriginalAddressTypeRole:
        return addresses_.at(index.row())->originalAddressType();
    case AddressRole:
        return addresses_.at(index.row())->address();
    case LabelRole:
        return addresses_.at(index.row())->label();
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
    lRoles[OriginalAddressTypeRole] = "originalAddressType";
    lRoles[AddressRole] = "address";
    lRoles[LabelRole] = "label";
    lRoles[PrimaryRole] = "primary";

    return lRoles;
}

void AccountAddresses::fromJSON(json::Value& list)
{
    for(size_t lIdx = 0; lIdx < list.size(); lIdx++)
    {
        json::Value lItem = list[lIdx];

        json::Value lAddressTypeItem;
        if(lItem.find(L"type", lAddressTypeItem))
        {
            AddressKeyFactory* lFactory = new AddressKeyFactory(Currency::type(lAddressTypeItem.getAString()));
            lFactory->fromJSON(lItem);
            factories_.insert(lFactory->type(), lFactory);
        }
    }

    makeAddresses();
}

void AccountAddresses::makeAddresses()
{
    qDeleteAll(addresses_);
    addresses_.clear();

    for(int lIdx = 0; lIdx < factories_.values().size(); lIdx++)
    {
        bool lFirst = true;
        AddressKeyFactory* lFactory = factories_.values()[lIdx];
        for(int lAddrIdx = 0; lAddrIdx < lFactory->keys().size(); lAddrIdx++)
        {
            AddressKey* lKey = lFactory->keys()[lAddrIdx];

            AccountAddress* lAddress = new AccountAddress(lKey->type(), lKey, !lFirst);
            addresses_.push_back(lAddress);
            lFirst = false;
        }
    }
}

void AccountAddresses::toJSON(json::Value& root)
{
    for(int lIdx = 0; lIdx < factories_.size(); lIdx++)
    {
        json::Value lItem = root.newArrayItem();
        AddressKeyFactory* lFactory = factories_.values()[lIdx];
        lFactory->toJSON(lItem);
    }
}

QString AccountAddresses::addAddress(QString addressType, QString label, bool primary)
{
    beginResetModel();

    AddressKeyFactory* lFactory = 0;
    QMap<Currency::Type, AddressKeyFactory*>::iterator lType = factories_.find(Currency::type(addressType.toStdString()));
    if (lType != factories_.end())
    {
        lFactory = lType.value();
    }
    else
    {
        lFactory = new AddressKeyFactory(Currency::type(addressType.toStdString()));
        factories_.insert(lFactory->type(), lFactory);
    }

    // make new one
    AddressKey* lKey = lFactory->newKey();
    lKey->setLabel(label);
    lKey->setPrimary(primary);

    // refill addresses array
    makeAddresses();

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

    qDeleteAll(factories_.values());
    factories_.clear();
}

AddressKeyFactory* AccountAddresses::getAddressFactory(Currency::Type type)
{
    QMap<Currency::Type, AddressKeyFactory*>::iterator lItem = factories_.find(type);
    if (lItem != factories_.end()) return lItem.value();
    return 0;
}

//
// Account
//
Account::Account(QObject *parent): QObject(parent),w_()
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
