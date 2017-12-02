#include "contacts.h"

using namespace gravio::wave;

//
// Contacts
//
Contacts::Contacts(QObject *parent): QAbstractListModel(parent)
{
    db_ = ContactsDbFactory::get();

    db_->open(QString(""));
    contacts_ = db_->selectAll();

    connect(db_, SIGNAL(dataChanged()), this, SLOT(dataChanged()));
}

void Contacts::dataChanged()
{
    beginResetModel();
    contacts_ = db_->select(filter_);
    endResetModel();

    emit listChanged();
}

int Contacts::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return contacts_.size();
}

QVariant Contacts::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    switch (role)
    {
    case NameRole:
        return contacts_.at(index.row())->name();
    case FullNameRole:
        return contacts_.at(index.row())->fullName();
    case AvatarRole:
        return contacts_.at(index.row())->avatar();
    case PrimaryAddressTypeRole:
        return Currency::name(contacts_.at(index.row())->primaryAddressType()).c_str();
    case PrimaryAddressRole:
        return contacts_.at(index.row())->primaryAddress();
    case AvatarSourceRole:
        return contacts_.at(index.row())->avatarSourcePath();
    default:
        return QVariant();
    }
}

QVector<int> Contacts::roles() const
{
    return QVector<int>() << NameRole << FullNameRole << AvatarRole << PrimaryAddressTypeRole << PrimaryAddressRole << AvatarSourceRole;
}

QHash<int, QByteArray> Contacts::roleNames() const
{
    QHash<int, QByteArray> lRoles = QAbstractListModel::roleNames();
    lRoles[NameRole] = "name";
    lRoles[FullNameRole] = "fullName";
    lRoles[AvatarRole] = "avatar";
    lRoles[PrimaryAddressTypeRole] = "primaryAddressType";
    lRoles[PrimaryAddressRole] = "primaryAddress";
    lRoles[AvatarSourceRole] = "avatarSource";

    return lRoles;
}

QString Contacts::removeContact(int index)
{
    if (index < contacts_.size())
    {
        beginResetModel();

        Contact* lContact = contacts_.at(index);
        contacts_.removeAt(index);
        db_->remove(lContact);

        endResetModel();

        return QString("");
    }

    return QString("Incorrect index");
}

void Contacts::applyFilter(QString filter)
{
    filter_ = filter;
    dataChanged();
}

void Contacts::resetFilter()
{
    filter_ = "";
    dataChanged();
}
