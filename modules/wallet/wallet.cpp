#include "wallet.h"

using namespace gravio::wave;

//
// Wallet
//
Wallet::Wallet(QObject *parent) : QObject(parent)
{
    units_ = 0;
    factory_ = 0;
    opened_ = false;

    QQmlApplicationEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Wallet::Wallet(IAccount* account, Currency::Type type, QObject *parent) : QObject(parent), account_(account), type_(type)
{
    units_ = 0;
    factory_ = 0;
    opened_ = false;

    QQmlApplicationEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Wallet::~Wallet()
{
    qDebug() << "Wallet::~Wallet()";

    backEnd_->StopSync();

    delay(1000);

    if (units_) delete units_;
    if (backEnd_) delete backEnd_;
    if (transactions_) delete transactions_;
}

void Wallet::delay(int msecs)
{
    QTime lTimeOut = QTime::currentTime().addMSecs(msecs);
    while (QTime::currentTime() < lTimeOut)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void Wallet::open()
{
    if (!opened_)
    {
        factory_ = account_->getAddressFactory(type_);
        units_ = new CurrencyUnits(factory_);
        backEnd_ = new backend::Wallet(factory_);
        transactions_ = new Transactions(this);

        connect(backEnd_, SIGNAL(newTransaction(uint256)), this, SLOT(transactionUpdated(uint256)));
        connect(backEnd_, SIGNAL(blockCountUpdated(uint64_t)), this, SLOT(blockCountUpdated(uint64_t)));

        opened_ = true;
    }
}

void Wallet::transactionUpdated(uint256 hash)
{
    std::map<uint256, backend::Transaction> lTxs = backEnd_->GetTransactions();
    std::map<uint256, backend::Transaction>::iterator lIterator = lTxs.find(hash);

    if (lIterator != lTxs.end())
    {
        transactions_->updateTransaction(lIterator->second);
    }
}

void Wallet::blockCountUpdated(uint64_t blockCount)
{
    transactions_->updateBlockCount();
}

QString Wallet::availableBalance()
{
    return units()->format(Currency::Unit::COIN, backEnd_->Balance(), false, CurrencyUnits::separatorAlways) + " " + QString::fromStdString(Currency::name(type_));
}

QString Wallet::pendingBalance()
{
    return QString("0.00000000 ") + QString::fromStdString(Currency::name(type_));
}

QString Wallet::totalBalance()
{
    return units()->format(Currency::Unit::COIN, backEnd_->Balance(), false, CurrencyUnits::separatorAlways) + " " + QString::fromStdString(Currency::name(type_));
}

