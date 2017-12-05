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

TransactionSync::TransactionSync()
{

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
            CTransaction tx;            
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