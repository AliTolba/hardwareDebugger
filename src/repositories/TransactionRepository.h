#pragma once

#include <QVector>

class DatabaseManager;

struct TransactionRecord
{
    int dId = -1;
    QString dChannel;
    QString dOperation;
    quint64 dAddress = 0;
    int dBurstLength = 0;
    QString dResponseCode;
    int dCycle = 0;
    QString dStatus;
};

class TransactionRepository
{
public:
    explicit TransactionRepository(DatabaseManager* xDatabase = nullptr);

    int mTotalCount() const;
    int mOpenCount() const;
    QVector<TransactionRecord> mFetchRows(int xOffset, int xLimit) const;
    bool mUpdateStatusAtRow(int xRow, const QString& xStatus) const;

private:
    DatabaseManager* dDatabase;
};
