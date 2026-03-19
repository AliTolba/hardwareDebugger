#pragma once

#include <functional>
#include <QStringList>
#include <QVariant>
#include <QVector>

class QObject;

struct TreeItemData
{
    int dId = -1;
    QVector<QVariant> dColumns;
    int dChildCount = 0;
};

class TreeViewServiceInterface
{
public:
    virtual ~TreeViewServiceInterface() = default;

    virtual QStringList mHeaders() const = 0;
    virtual int mItemCount(const QVariant& xParentId) const = 0;
    virtual QVector<TreeItemData> mFetchChildren(const QVariant& xParentId, int xOffset, int xLimit) const = 0;
    virtual bool mRequestItemCount(
        const QVariant& xParentId,
        QObject* xReceiver,
        const std::function<void(int)>& xOnLoaded) = 0;
    virtual bool mRequestChildren(
        const QVariant& xParentId,
        int xOffset,
        int xLimit,
        QObject* xReceiver,
        const std::function<void(QVector<TreeItemData>)>& xOnLoaded) = 0;

    virtual void mSetFilterPattern(const QString& xPattern) = 0;
    virtual void mSetSortColumn(int xColumnIndex, Qt::SortOrder xOrder) = 0;
    virtual void mClearFiltersAndSort() = 0;
};

class TableViewServiceInterface
{
public:
    virtual ~TableViewServiceInterface() = default;

    virtual QStringList mHeaders() const = 0;
    virtual int mTotalRowCount() const = 0;
    virtual QVector<QVector<QVariant>> mFetchRows(int xOffset, int xLimit) const = 0;
    virtual bool mRequestTotalRowCount(
        QObject* xReceiver,
        const std::function<void(int)>& xOnLoaded) = 0;
    virtual bool mRequestRows(
        int xOffset,
        int xLimit,
        QObject* xReceiver,
        const std::function<void(QVector<QVector<QVariant>>)>& xOnLoaded) = 0;

    virtual void mSetFilterPattern(const QString& xPattern) = 0;
    virtual void mSetSortColumn(int xColumnIndex, Qt::SortOrder xOrder) = 0;
    virtual void mClearFiltersAndSort() = 0;
};
