#include "wallet_transaction.h"
#include "wallet.h"

using namespace gravio::wave;

//
// Transaction
//
Transaction::Transaction(QObject *parent) : QObject(parent)
{
    time_ = 0;
    type_ = Type::Other;
    status_ = Status::Offline;

    QQmlApplicationEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Transaction::Transaction(Wallet* wallet, QObject *parent) : QObject(parent), wallet_(wallet)
{
    time_ = 0;
    type_ = Type::Other;
    status_ = Status::Offline;

    QQmlApplicationEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Transaction::~Transaction()
{
}

Wallet* Transaction::wallet() { return wallet_; }

int Transaction::confirmationState()
{
    double lTotal = wallet_->factory()->confirmations();
    int lPercent = (int)((double)blocks_ * 100.0) / lTotal;
    return lPercent;
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

void Transaction::create(backend::Transaction& tx)
{
    hash_ = tx.GetHash();
    address_ = QString::fromStdString(tx.GetAddress());
    debit_ = tx.GetDebit();
    credit_ = tx.GetCredit();

    int64_t lNet = credit_ - debit_;
    if (lNet > 0 || tx.IsCoinBase())
    {
        // credit
        if (!tx.IsCoinBase()) type_ = Type::RecvWithAddress;
        else type_ = Type::Generated;
    }
    else
    {
        // debit
        type_ = Type::SendToAddress;
    }

    update(tx);
}

void Transaction::update(backend::Transaction& tx)
{
    time_ = tx.GetTime();
    block_ = tx.nLockTime;

    updateStatus();
}

bool Transaction::updateStatus()
{
    if (status_ == Status::Confirmed) return false;

    blocks_ = wallet_->blocksCount() - block_; // mature blocks

    if (type_ == Type::Generated)
    {
        if (blocks_ < wallet_->factory()->maturity()) status_ = Status::Immature;
        else status_ = Status::Confirmed;
    }
    else
    {
        if (!blocks_) status_ = Status::Unconfirmed;
        else if (blocks_ < wallet_->factory()->confirmations()) status_ = Status::Confirming;
        else status_ = Status::Confirmed;
    }

    return true;
}

QString Transaction::formatAmount()
{
    return wallet_->units()->format(Currency::Unit::COIN, debit_ + credit_, true, CurrencyUnits::separatorAlways);
}
