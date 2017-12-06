#include "walletstore.h"
#include <stdio.h>
#include <string.h>

using namespace gravio::wave;

Wallet::Wallet(const Currency::Type & t = Currency::GIO):ctx(t), txstore(&ctx, &keystore)
{
    ctx.CreateSign();
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

WalletStore::WalletStore()
{

}

Wallet WalletStore::NewWallet(const Currency::Type& t)
{
    return Wallet(t);
}
