#include "wallet_transaction.h"

using namespace gravio::wave;

//
// Transaction
//
Transaction::Transaction(QObject *parent) : QObject(parent)
{
    QQmlApplicationEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Transaction::Transaction(Wallet* wallet, QObject *parent) : QObject(parent), wallet_(wallet)
{
    QQmlApplicationEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Transaction::~Transaction()
{
}

QString Transaction::tokenizeStatus()
{
    switch (status_)
    {
    case Transaction::Status::Confirmed: return QString("confirmed");
    case Transaction::Status::OpenUntilDate: return QString("open_until_date");
    case Transaction::Status::OpenUntilBlock: return QString("open_until_block");
    case Transaction::Status::Offline: return QString("offline");
    case Transaction::Status::Unconfirmed: return QString("unconfirmed");
    case Transaction::Status::Confirming: return QString("confirming");
    case Transaction::Status::Conflicted: return QString("conflicted");
    case Transaction::Status::Abandoned: return QString("abandoned");
    case Transaction::Status::Immature: return QString("immature");
    case Transaction::Status::MaturesWarning: return QString("matures_warning");
    case Transaction::Status::NotAccepted: return QString("not_accepted");
    }

    return QString("unknown");
}

QString Transaction::tokenizeType()
{
    switch(type_)
    {
    case Transaction::Type::Other: return QString("other");
    case Transaction::Type::Generated: return QString("generated");
    case Transaction::Type::SendToAddress: return QString("send_to_address");
    case Transaction::Type::SendToOther: return QString("send_to_other");
    case Transaction::Type::RecvWithAddress: return QString("recv_with_address");
    case Transaction::Type::RecvFromOther: return QString("recv_from_other");
    case Transaction::Type::SendToSelf: return QString("send_to_self");
    }

    return QString("unknown");
}
