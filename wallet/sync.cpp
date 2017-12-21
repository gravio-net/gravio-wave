#include "sync.h"
#include "utilstrencodings.h"
#include "datastream.h"
#include "primitives/transaction.h"
#include <vector>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

using namespace gravio::wave::backend;


bool DecodeHexTx(CTransaction& tx, const std::string& strHexTx, bool fTryNoWitness = false)
{
    if (!IsHex(strHexTx))
        return false;

    std::vector<unsigned char> txData(ParseHex(strHexTx));

    if (fTryNoWitness) {
        CDataStream ssData(txData, SER_NETWORK, PROTOCOL_VERSION | SERIALIZE_TRANSACTION_NO_WITNESS);
        try {
            ssData >> tx;
            if (ssData.eof()) {
                return true;
            }
        }
        catch (const std::exception&) {
            // Fall through.
        }
    }

    CDataStream ssData(txData, SER_NETWORK, PROTOCOL_VERSION);
    try {
        ssData >> tx;
    }
    catch (const std::exception&) {
        return false;
    }

    return true;
}

DataSync::DataSync()
{
    manager = new QNetworkAccessManager(this);
    //connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(Finished(QNetworkReply*)));
}

void DataSync::SendRequest(const Request& r)
{
    QString url = QString::fromStdString(r.Url);
    reply =  manager->get(QNetworkRequest(QUrl(url)));
    connect(reply, SIGNAL(finished()), this, SLOT(ReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
    qInfo() << "DataSync make request " <<url;
}

void DataSync::Finished(QNetworkReply* reply)
{
    qInfo() << "DataSync request complete";
    //QByteArray arr = reply->readAll();
    //qInfo() << QString::fromStdString(arr.toStdString());
    //reply->deleteLater();
    //emit RequestComplete(reply);
}

void DataSync::ReadyRead()
{
    qInfo() << "Readall start";
    QByteArray arr = reply->readAll();
    qInfo() << "Readall finished";
    emit RequestComplete(arr);
}

void DataSync::slotError(QNetworkReply::NetworkError err)
{
    //show error inf

    qInfo() << "NetworkError";
    emit RequestError(err);
}

/*void DataSync::RequestComplete(QNetworkReply* reply)
{
    ;
}*/

TransactionSync::TransactionSync(Context* c, DataSync* s, TransactionStore* st, IAddressKeyFactory* f):ctx(c), sync(s), store(st, f), factory(f)
{
    timer = new QTimer(this);
    processing = false;

    QList<IAddressKey*> keyslist = factory->keys();
    for(QList<IAddressKey*>::iterator it = keyslist.begin(); it != keyslist.end(); it++)
    {
        PubKey pk = (*it)->pubKey();
        uint160 id = pk.GetID();
        CryptoAddress addr(ctx, id);
        addresses.push_back(addr.ToString());
    }
    connect(timer, SIGNAL(timeout()), this, SLOT(StartSync()));
    connect(sync, SIGNAL(RequestComplete(QByteArray)),
            this, SLOT(RequestFinished(QByteArray)));
    connect(sync, SIGNAL(RequestError(QByteArray)),
            this, SLOT(RequestError(QByteArray)));
    StartSync();
    timer->start(10000);
}

void TransactionSync::StartSync()
{
    if(processing)
        return;
    addresses_queue = addresses;

    std::string url = ctx->BlockCountUrl();
    qInfo() << "start sync " << url.size();
    Request r;
    r.Url = ctx->BlockCountUrl();
    if(r.Url.size() > 0)
    {
        state = blocks_count;
        processing = true;
        sync->SendRequest(r);
    }
    else
    {
        qInfo() << "start sync complete url is empty";
        return;
    }

    qInfo() << "start sync complete";
}

void TransactionSync::RequestFinished(QByteArray arr)
{
    if(!processing) return;
    qInfo() << "TransactionSync request complete";
    qInfo() << QString::fromStdString(arr.toStdString());
    if(state == blocks_count)
    {
        qInfo() << "Blocks count result " << QString::fromStdString(arr.toStdString());
        uint64_t bc = arr.toUInt();
        if(bc != store->BlocksCount()) emit blockCountUpdated(bc);
        store->SetBlocksCount(bc);
        state = txlist;
        Request r;
        current_address = addresses_queue.front();
        addresses_queue.pop_front();
        r.Url = ctx->TransactionsListUrl() + current_address;
        sync->SendRequest(r);
    }
    else if(state == txlist)
    {
        qInfo() << "Transactions list result " << QString::fromStdString(arr.toStdString());
        QJsonDocument doc(QJsonDocument::fromJson(arr));
        QJsonObject json = doc.object();
        QJsonArray txs = json["last_txs"].toArray();
        if(!json["balance"].isNull())
        {
            double b = json["balance"].toDouble();
            CAmount bc = b * COIN;
            if(store->Balance() != bc)
                store->SetBalance(bc);
        }
        for (int i = 0; i < txs.size(); ++i)
        {
            QJsonObject tx = txs[i].toObject();
            std::string txhash = tx["addresses"].toString().toStdString();
            qInfo() << "Check tx hash " << QString::fromStdString(txhash);
            if(!store->HasTx(txhash))
            {
                qInfo() << "not found";
                std::string urltx = ctx->TransactionUrl() + "?txid="
                        + tx["addresses"].toString().toStdString() + "&decrypt=0";
                queue.push_back(urltx);
            }
            //std::vector<unsigned char> hash = ParseHex(txhash);
            //uint256 id(hash);
            uint256  id;
            id.SetHex(txhash);
            qInfo() << QString::fromStdString(id.ToString());
        }
        if(queue.size())
        {
            std::string url = queue.front();
            queue.pop_front();
            state = tx;
            Request r;
            r.Url = url;
            sync->SendRequest(r);
        }
        else
        {
            processing = false;
        }
    }
    else if(state == tx)
    {
        qInfo() << "Transaction result " << QString::fromStdString(arr.toStdString());
        Transaction t(store);
        if (DecodeHexTx(t, arr.toStdString()))
        {
            t.SetAddress(current_address);
            store->AddTx(t);
            emit newTransaction(t.GetHash());
            qInfo() << "Transaction decode " << QString::fromStdString(t.ToString());
        }
        if(queue.size())
        {
            std::string url = queue.front();
            queue.pop_front();
            state = tx;
            Request r;
            r.Url = url;
            sync->SendRequest(r);
        }
        else
        {
            if(addresses_queue.size())
            {
                state = txlist;
                Request r;
                std::string addr = addresses_queue.front();
                addresses_queue.pop_front();
                r.Url = ctx->TransactionsListUrl() + addr;
                sync->SendRequest(r);
            }
            else
                processing = false;
        }
    }
}

void TransactionSync::RequestError(QNetworkReply::NetworkError err)
{
    processing = false;
    qInfo() << "TransactionSync request error";
}

std::string TransactionSync::SyncWait(Context* ctx, TransactionStore* store, std::string address)
{
    QByteArray arr;
    QEventLoop loop;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            &loop, SLOT(quit()));
    QNetworkReply *reply;
    QString url = QString::fromStdString(ctx->TransactionsListUrl()) + QString::fromStdString(address);
    reply = manager->get(QNetworkRequest(QUrl(url)));
    loop.exec();
    reply->deleteLater();
    arr = reply->readAll();
    //return arr.toStdString();
    QJsonDocument doc(QJsonDocument::fromJson(arr));
    QJsonObject json = doc.object();
    QJsonArray txs = json["last_txs"].toArray();
    for (int i = 0; i < txs.size(); ++i)
    {
        QJsonObject tx = txs[i].toObject();
        if(!store->HasTx(tx["addresses"].toString().toStdString()))
        {
            //request tx
            //QEventLoop looptx;
            //QNetworkAccessManager *managertx = new QNetworkAccessManager(this);
            //connect(managertx, SIGNAL(finished(QNetworkReply*)),
            //        &looptx, SLOT(quit()));
            QNetworkReply *replytx;
            QString urltx = QString::fromStdString(ctx->TransactionUrl()) + QString("?txid=")
                    + tx["addresses"].toString() + QString("&decrypt=0");
            //return urltx.toStdString();
            replytx = manager->get(QNetworkRequest(QUrl(urltx)));
            loop.exec();
            replytx ->deleteLater();
            QByteArray arrtx = replytx->readAll();
            Transaction tx;
            if (DecodeHexTx(tx, arrtx.toStdString()))
            {
                store->AddTx(tx);
            }
            //return arrtx.toStdString();
        }
        //return tx["addresses"].toString().toStdString();
    }
    return json["balance"].toString().toStdString();
    //return arr.toStdString();
}
