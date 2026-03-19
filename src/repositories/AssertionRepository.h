#pragma once

#include <QVector>

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

private:
    DatabaseManager* dDatabase;
};
