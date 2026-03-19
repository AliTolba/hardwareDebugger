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
    const QString lWhereClause = mBuildWhereClause();
    const QString lQueryStr = QStringLiteral("SELECT COUNT(*) FROM assertions") +
                             (lWhereClause.isEmpty() ? QString() : QStringLiteral(" WHERE ") + lWhereClause);
    lQuery.exec(lQueryStr);
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

    const QString lWhereClause = mBuildWhereClause();
    const QString lOrderByClause = mBuildOrderByClause();

    QSqlQuery lQuery(dDatabase->mConnection());
    QString lQueryStr = QStringLiteral(
        "SELECT id, property_name, module_name, severity, status, message, cycle, time_ns "
        "FROM assertions");

    if (!lWhereClause.isEmpty()) {
        lQueryStr += QStringLiteral(" WHERE ") + lWhereClause;
    }

    lQueryStr += QStringLiteral(" ") + lOrderByClause;
    lQueryStr += QStringLiteral(" LIMIT :limit OFFSET :offset");

    lQuery.prepare(lQueryStr);
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

void AssertionRepository::mSetFilterPattern(const QString& xPattern)
{
    dFilterPattern = xPattern;
}

void AssertionRepository::mSetSortColumn(int xColumnIndex, Qt::SortOrder xOrder)
{
    dSortColumnIndex = xColumnIndex;
    dSortOrder = xOrder;
}

void AssertionRepository::mClearFiltersAndSort()
{
    dFilterPattern.clear();
    dSortColumnIndex = -1;
    dSortOrder = Qt::AscendingOrder;
}

QString AssertionRepository::mBuildWhereClause() const
{
    if (dFilterPattern.isEmpty()) {
        return QString();
    }

    // Build a WHERE clause that searches all columns
    const QStringList lColumns = {
        QStringLiteral("property_name"),
        QStringLiteral("module_name"),
        QStringLiteral("severity"),
        QStringLiteral("status"),
        QStringLiteral("message")
    };

    QStringList lConditions;
    for (const QString& lColumn : lColumns) {
        lConditions.append(lColumn + QStringLiteral(" LIKE '%") + dFilterPattern + QStringLiteral("%'"));
    }

    return lConditions.join(QStringLiteral(" OR "));
}

QString AssertionRepository::mBuildOrderByClause() const
{
    constexpr int sColumnCount = 7;
    const QStringList lColumns = {
        QStringLiteral("id"),
        QStringLiteral("property_name"),
        QStringLiteral("module_name"),
        QStringLiteral("severity"),
        QStringLiteral("status"),
        QStringLiteral("message"),
        QStringLiteral("cycle"),
        QStringLiteral("time_ns")
    };

    if (dSortColumnIndex >= 0 && dSortColumnIndex < lColumns.size()) {
        const QString lColumnName = lColumns.at(dSortColumnIndex);
        const QString lOrderDir = (dSortOrder == Qt::AscendingOrder) ? QStringLiteral("ASC") : QStringLiteral("DESC");
        return QStringLiteral("ORDER BY ") + lColumnName + QStringLiteral(" ") + lOrderDir;
    }

    return QStringLiteral("ORDER BY id ASC");
}
