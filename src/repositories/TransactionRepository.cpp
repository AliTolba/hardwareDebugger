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
    lQuery.exec(QStringLiteral("SELECT COUNT(*) FROM transactions"));
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

    QSqlQuery lQuery(dDatabase->mConnection());
    lQuery.prepare(QStringLiteral(
        "SELECT id, channel, operation, address, burst_length, response_code, cycle, status "
        "FROM transactions ORDER BY id LIMIT :limit OFFSET :offset"));
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
