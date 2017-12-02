
#include <QDir>
#include <QFile>
#include <QStandardPaths>

#include "../../module.h"
#include "contactsdb.h"

using namespace gravio::wave;

ContactsDb* gContactsDb = 0;

ContactsDb* ContactsDbFactory::get()
{
    if (!gContactsDb) gContactsDb = new ContactsJSONDb();
    return gContactsDb;
}

/*
 * ./db.json
 * {
 *      "list":
 *      [
 *          {
 *              "id": 1,
 *              "name": "short name",
 *              "fullName": "full name",
 *              "avatar": "avatar.jpg",
 *              "addresses":
 *              [
 *                  {
 *                      "addressType": "GIO",
 *                      "address": "2Murbwjkkcnke3jek9mlxdLJkdldlkl33",
 *                      "label": "Primary address",
 *                      "pubKey": "lkjdlfkjgjrijejn8978klj3lkjlkjdflk",
 *                      "primary": true
 *                  }
 *              ]
 *          }
 *      ]
 * }
 *
 */

QString ContactsJSONDb::getFile()
{
    QString lGlobalDataPath = ApplicationPath::dataDirPath();
    QString lModuleDataPath = lGlobalDataPath + "/contacts";

    QDir lCurrentDataDir(lModuleDataPath);
    if (!lCurrentDataDir.exists())
    {
        lCurrentDataDir.setPath(lGlobalDataPath);
        lCurrentDataDir.mkdir("contacts");
    }

    QString lDbFile = lModuleDataPath + "/db.json";
    return lDbFile;
}

void ContactsJSONDb::open(QString /*secret*/)
{
    QString lDbFile = getFile();

    //
    // TODO: AES decode using 'secret'
    //
    json::Document lDb;
    lDb.loadFromFile(lDbFile.toStdString());

    json::Value lList;
    if (lDb.find(L"list", lList))
    {
        if (lList.isArray())
        {
            for (size_t lIdx = 0; lIdx < lList.size(); lIdx++)
            {
                Contact* lContact = new Contact(); // new
                lContact->link(this); // link to db

                const json::Value& lItem = lList[lIdx];
                lContact->fromJSON(const_cast<json::Value&>(lItem));
                pushItem(lContact);

                identity_ = identity_ < lContact->id() ? lContact->id() : identity_;
            }
        }
    }
}

void ContactsJSONDb::pushItem(Contact* item)
{
    if (nameIndex_.contains(item->name())) throw Exception(std::string("Contact '") + item->name().toStdString() + std::string("' aready exists."));

    cache_.insert(item->id(), item);
    nameIndex_.insert(item->name(), item->id());
}

void ContactsJSONDb::popItem(Contact* item)
{
    Contact* lContact = cache_.value(item->id(), 0);
    if (lContact)
    {
        cache_.remove(item->id());
        nameIndex_.remove(item->name());

        delete lContact;
    }
}

void ContactsJSONDb::insert(Contact* item)
{
    item->setId(nextId());
    pushItem(item);
    saveAll();

    emit dataChanged();
}

void ContactsJSONDb::update(Contact* item)
{
    if (!item->id()) insert(item);
    else
    {
        nameIndex_.clear();

        for(QMap<int, Contact*>::iterator lItem = cache_.begin(); lItem != cache_.end(); lItem++)
        {
            nameIndex_.insert(lItem.value()->name(), lItem.value()->id());
        }

        saveAll();
    }

    emit dataChanged();
}

void ContactsJSONDb::remove(Contact* item)
{
    popItem(item);
    saveAll();

    emit dataChanged();
}

QList<Contact*> ContactsJSONDb::selectAll()
{
    QList<Contact*> lList;

    for(QMap<QString, int>::iterator lItem = nameIndex_.begin(); lItem != nameIndex_.end(); lItem++)
    {
        lList.push_back(cache_[lItem.value()]);
    }

    return lList;
}

QList<Contact*> ContactsJSONDb::select(const QString& filter)
{
    if (!filter.length()) return selectAll();

    QMap<QString, Contact*> lList;
    for(QMap<int, Contact*>::iterator lItem = cache_.begin(); lItem != cache_.end(); lItem++)
    {
        if (lItem.value()->name().contains(filter, Qt::CaseSensitivity::CaseInsensitive) ||
                lItem.value()->fullName().contains(filter, Qt::CaseSensitivity::CaseInsensitive) ||
                lItem.value()->getAddresses()->contains(filter))
        {
            lList.insert(lItem.value()->name(), lItem.value());
        }
    }

    return lList.values();
}

void ContactsJSONDb::saveAll()
{
    json::Document lDb;
    lDb.loadFromString("{}");

    const json::Value& lList = lDb.addArray(L"list");
    QList<Contact*> lValues = cache_.values();

    for (int lIdx = 0; lIdx < lValues.length(); lIdx++)
    {
        Contact* lContact = lValues.at(lIdx);
        const json::Value& lItem = lList.newArrayItem();
        lContact->toJSON(const_cast<json::Value&>(lItem));
    }

    QString lDbFile = getFile();
    lDb.saveToFile(lDbFile.toStdString());
}
