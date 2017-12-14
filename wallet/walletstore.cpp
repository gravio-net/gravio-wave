#include "walletstore.h"
#include <stdio.h>
#include <string.h>
#include <QFile>
#include "application.h"
#include "exception.h"

using namespace gravio::wave::backend;

Wallet::Wallet(const Currency::Type & t):ctx(t), txstore(&ctx, &keystore), sync(),txsync(&ctx, &sync)
{
    ctx.CreateSign();
    qInfo() << "Starting wallet";
}

Key Wallet::NewKey()
{
    Key k(&ctx);
    k.MakeNew();
    return k;
}

void Wallet::SetKey(Key& k)
{
    keystore.SetKey(k);
}

void Wallet::AddPubKey(PubKey& pk)
{
    keystore.AddPubKey(pk);
}

/*WalletStore::WalletStore()
{

}

Wallet WalletStore::NewWallet(const Currency::Type& t)
{
    return Wallet(t);
}*/
