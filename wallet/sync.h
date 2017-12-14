#ifndef SYNC_H
#define SYNC_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "context.h"
#include "transactionstore.h"
#include <string>

namespace gravio {
namespace wave {
namespace backend {

class Request
{
public:
    std::string Url;
};

class DataSync : public QObject
{
    Q_OBJECT
public:

    DataSync();
    void SendRequest(const Request&);
public slots:
    void Finished(QNetworkReply* reply);
    void ReadyRead();
    void slotError(QNetworkReply::NetworkError);
signals:
    void RequestComplete(QByteArray arr);
    void RequestError(QNetworkReply::NetworkError);

private:
    QNetworkAccessManager *manager;
    QNetworkReply* reply;
};

enum ProcessState
{
    blocks_count,
    txlist,
    tx
};

class TransactionSync : public QObject
{
    Q_OBJECT
public:

    TransactionSync(Context* c, DataSync* s, TransactionStore* store);
    virtual ~TransactionSync(){}
    std::string SyncWait(Context* ctx, TransactionStore* store, std::string address);
public slots:
    void StartSync();
    void RequestFinished(QByteArray arr);
    void RequestError(QNetworkReply::NetworkError);
private:

    Context* ctx;
    DataSync* sync;
    TransactionStore* store;
    QTimer* timer;
    bool processing;
    ProcessState state;
    std::list<Request> queue;
};

} //backend
} // wave
} // gravio

#endif // SYNC_H
