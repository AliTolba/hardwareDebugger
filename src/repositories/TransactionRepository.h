#pragma once

#include <QVector>
#include <Qt>

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

    void mSetFilterPattern(const QString& xPattern);
    void mSetSortColumn(int xColumnIndex, Qt::SortOrder xOrder);
    void mClearFiltersAndSort();

private:
    QString mBuildWhereClause() const;
    QString mBuildOrderByClause() const;

    DatabaseManager* dDatabase;
    QString dFilterPattern;
    int dSortColumnIndex = -1;
    Qt::SortOrder dSortOrder = Qt::AscendingOrder;
};
