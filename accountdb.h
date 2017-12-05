#ifndef ACCOUNTDB_H
#define ACCOUNTDB_H

#include <QString>
#include <QList>
#include <QMap>

#include "account.h"
#include "json.h"

namespace gravio {
namespace wave {

/**
 * @brief The AccountDb class
 * AccountDb interface
 */
class AccountDb: public QObject
{
    Q_OBJECT
public:
    AccountDb(QObject *parent = 0): QObject(parent) {}
    virtual ~AccountDb() {}

    virtual void open(QString) = 0;
    virtual Account* select() = 0;
    virtual void update() = 0;
    virtual void link(Account*) = 0;

signals:
    void dataChanged();
};

/**
 * @brief The AccountJSONDb class
 * AccountJSONDb implementation
 */
class AccountJSONDb: public AccountDb
{
    Q_OBJECT
public:
    AccountJSONDb() { account_ = 0; opened_ = false; }
    ~AccountJSONDb() {}

    void open(QString);
    Account* select();
    void update();
    void link(Account*);

private:
    QString getFile();

private:
    Account* account_;
    bool opened_;
};

/**
 * @brief The AccountDbFactory class
 * Db factory implementation
 */
class AccountDbFactory
{
public:
    static AccountDb* get();
};

} // wave
} // gravio

#endif // ACCOUNTDB_H
