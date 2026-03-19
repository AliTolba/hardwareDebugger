#pragma once

#include <QStringList>
#include <QVariant>
#include <QVector>

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
};

class TableViewServiceInterface
{
public:
    virtual ~TableViewServiceInterface() = default;

    virtual QStringList mHeaders() const = 0;
    virtual int mTotalRowCount() const = 0;
    virtual QVector<QVector<QVariant>> mFetchRows(int xOffset, int xLimit) const = 0;
};
