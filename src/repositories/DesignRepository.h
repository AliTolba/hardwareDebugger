#pragma once

#include <QVariant>
#include <QVector>
#include <Qt>

class DatabaseManager;

struct DesignNodeRecord
{
    int dId = -1;
    QString dInstanceName;
    QString dNodeKind;
    QString dSourceFile;
    int dChildCount = 0;
};

class DesignRepository
{
public:
    explicit DesignRepository(DatabaseManager* xDatabase = nullptr);

    int mTotalCount() const;
    int mItemCount(const QVariant& xParentId) const;
    QVector<DesignNodeRecord> mFetchChildren(const QVariant& xParentId, int xOffset, int xLimit) const;

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
