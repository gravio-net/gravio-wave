#ifndef SYNC_H
#define SYNC_H

#include <QObject>
#include <QString>
#include "context.h"
#include "transactionstore.h"

class TransactionSync : public QObject
{
public:
    TransactionSync();
    std::string SyncWait(Context* ctx, TransactionStore* store, std::string address);
};


#endif // SYNC_H
