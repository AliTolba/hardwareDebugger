#include "AssertionRepository.h"

#include "DatabaseManager.h"

#include <QSqlQuery>

AssertionRepository::AssertionRepository(DatabaseManager* xDatabase)
    : dDatabase(xDatabase)
{
}

int AssertionRepository::mTotalCount() const
{
    if (dDatabase == nullptr) {
        return 0;
    }

    QSqlQuery lQuery(dDatabase->mConnection());
    lQuery.exec(QStringLiteral("SELECT COUNT(*) FROM assertions"));
    return lQuery.next() ? lQuery.value(0).toInt() : 0;
}

int AssertionRepository::mFailureCount() const
{
    if (dDatabase == nullptr) {
        return 0;
    }

    QSqlQuery lQuery(dDatabase->mConnection());
    lQuery.exec(QStringLiteral("SELECT COUNT(*) FROM assertions WHERE status = 'FAIL'"));
    return lQuery.next() ? lQuery.value(0).toInt() : 0;
}

QVector<AssertionRecord> AssertionRepository::mFetchRows(int xOffset, int xLimit) const
{
    QVector<AssertionRecord> lRows;
    if (dDatabase == nullptr) {
        return lRows;
    }

    QSqlQuery lQuery(dDatabase->mConnection());
    lQuery.prepare(QStringLiteral(
        "SELECT id, property_name, module_name, severity, status, message, cycle, time_ns "
        "FROM assertions ORDER BY id LIMIT :limit OFFSET :offset"));
    lQuery.bindValue(QStringLiteral(":limit"), xLimit);
    lQuery.bindValue(QStringLiteral(":offset"), xOffset);
    if (!lQuery.exec()) {
        return lRows;
    }

    while (lQuery.next()) {
        AssertionRecord lRow;
        lRow.dId = lQuery.value(0).toInt();
        lRow.dPropertyName = lQuery.value(1).toString();
        lRow.dModuleName = lQuery.value(2).toString();
        lRow.dSeverity = lQuery.value(3).toString();
        lRow.dStatus = lQuery.value(4).toString();
        lRow.dMessage = lQuery.value(5).toString();
        lRow.dCycle = lQuery.value(6).toInt();
        lRow.dTimeNs = lQuery.value(7).toLongLong();
        lRows.append(lRow);
    }

    return lRows;
}
