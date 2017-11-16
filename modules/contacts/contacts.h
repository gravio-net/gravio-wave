#ifndef CONTACTS_H
#define CONTACTS_H

#include <QQuickItem>

#define URI "net.gravio.wave.modules.contacts"
#define NAME "Contacts"

class ContactsViewModel : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(ContactsViewModel)

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)

public:
    ContactsViewModel(QQuickItem *parent = nullptr);
    ~ContactsViewModel();

    QString title() const;

Q_SIGNALS:
    void titleChanged();
};

#endif // CONTACTS_H
