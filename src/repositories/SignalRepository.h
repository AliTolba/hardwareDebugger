#pragma once

#include <QVariant>
#include <QVector>

class DatabaseManager;

struct SignalNodeRecord
{
    int dId = -1;
    QString dSignalName;
    QString dDirection;
    int dWidthBits = 0;
    QString dDomainName;
    int dChildCount = 0;
};

class SignalRepository
{
public:
    explicit SignalRepository(DatabaseManager* xDatabase = nullptr);

    int mTotalCount() const;
    int mItemCount(const QVariant& xParentId) const;
    QVector<SignalNodeRecord> mFetchChildren(const QVariant& xParentId, int xOffset, int xLimit) const;

private:
    DatabaseManager* dDatabase;
};
