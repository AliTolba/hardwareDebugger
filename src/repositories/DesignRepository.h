#pragma once

#include <QVariant>
#include <QVector>

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

private:
    DatabaseManager* dDatabase;
};
