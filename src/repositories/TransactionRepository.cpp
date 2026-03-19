#include "TransactionRepository.h"

#include "DatabaseManager.h"

#include <QSqlQuery>

TransactionRepository::TransactionRepository(DatabaseManager* xDatabase)
    : dDatabase(xDatabase)
{
}

int TransactionRepository::mTotalCount() const
{
    if (dDatabase == nullptr) {
        return 0;
    }

    QSqlQuery lQuery(dDatabase->mConnection());
    const QString lWhereClause = mBuildWhereClause();
    const QString lQueryStr = QStringLiteral("SELECT COUNT(*) FROM transactions") +
                             (lWhereClause.isEmpty() ? QString() : QStringLiteral(" WHERE ") + lWhereClause);
    lQuery.exec(lQueryStr);
    return lQuery.next() ? lQuery.value(0).toInt() : 0;
}

int TransactionRepository::mOpenCount() const
{
    if (dDatabase == nullptr) {
        return 0;
    }

    QSqlQuery lQuery(dDatabase->mConnection());
    lQuery.exec(QStringLiteral("SELECT COUNT(*) FROM transactions WHERE status IN ('Pending', 'Retry')"));
    return lQuery.next() ? lQuery.value(0).toInt() : 0;
}

QVector<TransactionRecord> TransactionRepository::mFetchRows(int xOffset, int xLimit) const
{
    QVector<TransactionRecord> lRows;
    if (dDatabase == nullptr) {
        return lRows;
    }

    const QString lWhereClause = mBuildWhereClause();
    const QString lOrderByClause = mBuildOrderByClause();

    QSqlQuery lQuery(dDatabase->mConnection());
    QString lQueryStr = QStringLiteral(
        "SELECT id, channel, operation, address, burst_length, response_code, cycle, status "
        "FROM transactions");

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
        TransactionRecord lRow;
        lRow.dId = lQuery.value(0).toInt();
        lRow.dChannel = lQuery.value(1).toString();
        lRow.dOperation = lQuery.value(2).toString();
        lRow.dAddress = lQuery.value(3).toULongLong();
        lRow.dBurstLength = lQuery.value(4).toInt();
        lRow.dResponseCode = lQuery.value(5).toString();
        lRow.dCycle = lQuery.value(6).toInt();
        lRow.dStatus = lQuery.value(7).toString();
        lRows.append(lRow);
    }

    return lRows;
}

bool TransactionRepository::mUpdateStatusAtRow(int xRow, const QString& xStatus) const
{
    if (dDatabase == nullptr || xRow < 0) {
        return false;
    }

    QSqlQuery lQuery(dDatabase->mConnection());
    lQuery.prepare(QStringLiteral(
        "UPDATE transactions SET status = :status "
        "WHERE id = (SELECT id FROM transactions ORDER BY id LIMIT 1 OFFSET :offset)"));
    lQuery.bindValue(QStringLiteral(":status"), xStatus);
    lQuery.bindValue(QStringLiteral(":offset"), xRow);

    if (!lQuery.exec()) {
        return false;
    }

    return lQuery.numRowsAffected() > 0;
}

void TransactionRepository::mSetFilterPattern(const QString& xPattern)
{
    dFilterPattern = xPattern;
}

void TransactionRepository::mSetSortColumn(int xColumnIndex, Qt::SortOrder xOrder)
{
    dSortColumnIndex = xColumnIndex;
    dSortOrder = xOrder;
}

void TransactionRepository::mClearFiltersAndSort()
{
    dFilterPattern.clear();
    dSortColumnIndex = -1;
    dSortOrder = Qt::AscendingOrder;
}

QString TransactionRepository::mBuildWhereClause() const
{
    if (dFilterPattern.isEmpty()) {
        return QString();
    }

    const QStringList lColumns = {
        QStringLiteral("channel"),
        QStringLiteral("operation"),
        QStringLiteral("response_code"),
        QStringLiteral("status")
    };

    QStringList lConditions;
    for (const QString& lColumn : lColumns) {
        lConditions.append(lColumn + QStringLiteral(" LIKE '%") + dFilterPattern + QStringLiteral("%'"));
    }

    return lConditions.join(QStringLiteral(" OR "));
}

QString TransactionRepository::mBuildOrderByClause() const
{
    const QStringList lColumns = {
        QStringLiteral("id"),
        QStringLiteral("channel"),
        QStringLiteral("operation"),
        QStringLiteral("address"),
        QStringLiteral("burst_length"),
        QStringLiteral("response_code"),
        QStringLiteral("cycle"),
        QStringLiteral("status")
    };

    if (dSortColumnIndex >= 0 && dSortColumnIndex < lColumns.size()) {
        const QString lColumnName = lColumns.at(dSortColumnIndex);
        const QString lOrderDir = (dSortOrder == Qt::AscendingOrder) ? QStringLiteral("ASC") : QStringLiteral("DESC");
        return QStringLiteral("ORDER BY ") + lColumnName + QStringLiteral(" ") + lOrderDir;
    }

    return QStringLiteral("ORDER BY id ASC");
}
