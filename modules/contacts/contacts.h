#ifndef CONTACTS_H
#define CONTACTS_H

#include <QQuickItem>
#include <QAbstractListModel>
#include <QList>
#include <QQmlApplicationEngine>

#include "contact.h"
#include "contactsdb.h"

namespace gravio {
namespace wave {

/**
 * @brief The Contacts class
 * Contacts model
 */
class Contacts: public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QList<Contact*> list READ getList NOTIFY listChanged)

public:
    enum ContactsRoles
    {
        NameRole = Qt::UserRole + 1,
        FullNameRole,
        AvatarRole,
        PrimaryAddressTypeRole,
        PrimaryAddressRole,
        AvatarSourceRole
    };

    Contacts(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;
    QVector<int> roles() const;

    QList<Contact*> getList() const { return contacts_; }

    Q_INVOKABLE QVariant newContact()
    {
        Contact* lContact = new Contact();
        lContact->link(db_);

        return QVariant::fromValue(lContact);
    }
    Q_INVOKABLE void update() { db_->saveAll(); }
    Q_INVOKABLE QVariant get(int index)
    {
        return QVariant::fromValue(contacts_.at(index));
    }
    Q_INVOKABLE QString removeContact(int index);
    Q_INVOKABLE void applyFilter(QString);
    Q_INVOKABLE void resetFilter();

signals:
    void listChanged();

public slots:
    void dataChanged();

private:
    QString filter_;
    QList<Contact*> contacts_;
    ContactsDb* db_;
};

} // wave
} // gravio

#endif // CONTACTS_H
