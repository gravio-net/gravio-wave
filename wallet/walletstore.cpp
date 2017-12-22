#include "walletstore.h"
#include <stdio.h>
#include <string.h>
#include <QFile>
#include "application.h"
#include "exception.h"
#include "iaccount.h"

using namespace gravio::wave::backend;

Wallet::Wallet(IAddressKeyFactory* f)
{
    txsync = 0;
    factory = f;
    ctx = factory->context();
    txstore = TransactionStore(ctx, f);
    txsync = new TransactionSync (ctx, &sync, &txstore, f);

    connect(txsync, SIGNAL(newTransaction(uint256)),
            this, SLOT(slotNewTransaction(uint256)));
    connect(txsync, SIGNAL(blockCountUpdated(uint64_t)),
            this, SLOT(slotBlockCountUpdated(uint64_t)));

    qInfo() << "Starting wallet";
}

Wallet::~Wallet()
{
    if(txsync)
        delete txsync;
}

void Wallet::slotNewTransaction(uint256 h)
{
    emit newTransaction(h);
}

void Wallet::slotBlockCountUpdated(uint64_t bc)
{
    emit blockCountUpdated(bc);
}

Key Wallet::NewKey()
{
    Key k(ctx);
    k.MakeNew();
    return k;
}


/*WalletStore::WalletStore()
{

}

Wallet WalletStore::NewWallet(const Currency::Type& t)
{
    return Wallet(t);
}*/
