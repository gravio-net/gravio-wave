
#include <QDir>
#include <QFile>
#include <QStandardPaths>

#include "module.h"
#include "accountdb.h"

using namespace gravio::wave;

AccountDb* gAccountDb = 0;

AccountDb* AccountDbFactory::get()
{
    if (!gAccountDb) gAccountDb = new AccountJSONDb();
    return gAccountDb;
}

/*
 * ./db.json
 * {
 *      "name": "short name",
 *      "fullName": "full name",
 *      "avatar": "avatar.jpg",
 *      "addresses":
 *      [
 *          {
 *              "addressType": "GIO",
 *              "address": "2Murbwjkkcnke3jek9mlxdLJkdldlkl33",
 *              "label": "Primary address",
 *              "pubKey": "lkjdlfkjgjrijejn8978klj3lkjlkjdflk",
 *              "primary": true
 *          }
 *      ]
 *      "privateKeys":
 *      [
 *          {
 *              "addressType": "GIO",
 *              "privateKey": "ksjdlfkjslkdfjskjfdlskdjflskjd"
 *          }
 *          {
 *              "addressType": "BTC",
 *              "privateKey": "ioyuiowhfeklhnwklfhlwkjehf"
 *          }
 *      ]
 * }
 *
 */

QString AccountJSONDb::getFile()
{
    QString lGlobalDataPath = ApplicationPath::dataDirPath();
    QString lModuleDataPath = lGlobalDataPath + "/account";

    QDir lCurrentDataDir(lModuleDataPath);
    if (!lCurrentDataDir.exists())
    {
        lCurrentDataDir.setPath(lGlobalDataPath);
        lCurrentDataDir.mkdir("account");
    }

    QString lDbFile = lModuleDataPath + "/db.json";
    return lDbFile;
}

void AccountJSONDb::link(Account* account)
{
    if (!account_)
    {
        account_ = account;
    }
}

void AccountJSONDb::open(QString /*secret*/)
{
    if (opened_) return;

    QString lDbFile = getFile();

    //
    // TODO: AES decode using 'secret'
    //
    json::Document lDb;
    lDb.loadFromFile(lDbFile.toStdString());

    json::Value lRoot;
    if (lDb.find(L"name", lRoot)) // sanity check
    {
        account_->fromJSON(lDb);
    }

    opened_ = true;
}

Account* AccountJSONDb::select()
{
    return account_;
}

void AccountJSONDb::update()
{
    //
    // TODO: AES encryption using 'secret'
    //

    json::Document lDb;
    lDb.loadFromString("{}");

    account_->toJSON(lDb);

    QString lDbFile = getFile();
    lDb.saveToFile(lDbFile.toStdString());
}
