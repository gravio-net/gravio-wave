#include "walletstore.h"
#include <stdio.h>
#include <string.h>

Wallet::Wallet(const WalletType & t = WalletType::GIO):ctx(t), txstore(&ctx, &keystore)
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

Wallet WalletStore::NewWallet(const WalletType& t)
{
    return Wallet(t);
}
