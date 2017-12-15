#include "walletstore.h"
#include <stdio.h>
#include <string.h>
#include <QFile>
#include "application.h"
#include "exception.h"

using namespace gravio::wave::backend;

Wallet::Wallet(IAddressKeyFactory* f)
{
    txsync = 0;
    factory = f;
    ctx = factory->context();
    txstore = TransactionStore(ctx);
    txsync = new TransactionSync (ctx, &sync, &txstore, f);

    qInfo() << "Starting wallet";
}

Wallet::~Wallet()
{
    if(txsync)
        delete txsync;
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
