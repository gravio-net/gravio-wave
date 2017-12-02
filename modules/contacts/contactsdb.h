#ifndef CONTACTSDB_H
#define CONTACTSDB_H

#include <QString>
#include <QList>
#include <QMap>

#include "contact.h"
#include "../../json.h"

namespace gravio {
namespace wave {

/**
 * @brief The ContactsDb class
 * ContactsDb interface
 */
class ContactsDb: public QObject
{
    Q_OBJECT
public:
    ContactsDb(QObject *parent = 0): QObject(parent) {}
    virtual ~ContactsDb() {}

    virtual void open(QString) = 0;
    virtual QList<Contact*> selectAll() = 0;
    virtual QList<Contact*> select(const QString&) = 0;
    virtual void saveAll() = 0;
    virtual void insert(Contact*) = 0;
    virtual void update(Contact*) = 0;
    virtual void remove(Contact*) = 0;

signals:
    void dataChanged();
};

/**
 * @brief The ContactsJSONDb class
 * ContactsJSONDb implementation
 */
class ContactsJSONDb: public ContactsDb
{
    Q_OBJECT
public:
    ContactsJSONDb() { identity_ = 0; /* by default */ }
    ~ContactsJSONDb() {}

    void open(QString);
    QList<Contact*> selectAll();
    QList<Contact*> select(const QString&);
    void saveAll();

    void insert(Contact*);
    void update(Contact*);
    void remove(Contact*);

private:
    QString getFile();
    int nextId() { return ++identity_; }

    void pushItem(Contact*);
    void popItem(Contact*);

private:
    QMap<int, Contact*> cache_;
    QMap<QString, int> nameIndex_;
    int identity_;
};

/**
 * @brief The ContactsDbFactory class
 * Db factory implementation
 */
class ContactsDbFactory
{
public:
    static ContactsDb* get();
};

} // wave
} // gravio

#endif // CONTACTSDB_H
