#pragma once

#include <QVector>
#include <Qt>

class DatabaseManager;

struct AssertionRecord
{
    int dId = -1;
    QString dPropertyName;
    QString dModuleName;
    QString dSeverity;
    QString dStatus;
    QString dMessage;
    int dCycle = 0;
    qint64 dTimeNs = 0;
};

class AssertionRepository
{
public:
    explicit AssertionRepository(DatabaseManager* xDatabase = nullptr);

    int mTotalCount() const;
    int mFailureCount() const;
    QVector<AssertionRecord> mFetchRows(int xOffset, int xLimit) const;

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
