#include "SignalRepository.h"

#include "DatabaseManager.h"

#include <QSqlQuery>

SignalRepository::SignalRepository(DatabaseManager* xDatabase)
    : dDatabase(xDatabase)
{
}

int SignalRepository::mTotalCount() const
{
    if (dDatabase == nullptr) {
        return 0;
    }

    QSqlQuery lQuery(dDatabase->mConnection());
    lQuery.exec(QStringLiteral("SELECT COUNT(*) FROM signal_nodes"));
    return lQuery.next() ? lQuery.value(0).toInt() : 0;
}

int SignalRepository::mItemCount(const QVariant& xParentId) const
{
    if (dDatabase == nullptr) {
        return 0;
    }

    QSqlQuery lQuery(dDatabase->mConnection());
    QString lSql = QStringLiteral("SELECT COUNT(*) FROM signal_nodes ");
    if (xParentId.isValid()) {
        lSql += QStringLiteral("WHERE parent_id = :parent_id");
    } else {
        lSql += QStringLiteral("WHERE parent_id IS NULL");
    }

    lQuery.prepare(lSql);
    if (xParentId.isValid()) {
        lQuery.bindValue(QStringLiteral(":parent_id"), xParentId);
    }

    if (!lQuery.exec()) {
        return 0;
    }

    return lQuery.next() ? lQuery.value(0).toInt() : 0;
}

QVector<SignalNodeRecord> SignalRepository::mFetchChildren(const QVariant& xParentId, int xOffset, int xLimit) const
{
    QVector<SignalNodeRecord> lRows;
    if (dDatabase == nullptr) {
        return lRows;
    }

    QSqlQuery lQuery(dDatabase->mConnection());
    QString lSql = QStringLiteral(
        "SELECT id, signal_name, direction, width_bits, domain_name, child_count "
        "FROM signal_nodes ");
    if (xParentId.isValid()) {
        lSql += QStringLiteral("WHERE parent_id = :parent_id ");
    } else {
        lSql += QStringLiteral("WHERE parent_id IS NULL ");
    }
    lSql += QStringLiteral("ORDER BY id LIMIT :limit OFFSET :offset");

    lQuery.prepare(lSql);
    if (xParentId.isValid()) {
        lQuery.bindValue(QStringLiteral(":parent_id"), xParentId);
    }
    lQuery.bindValue(QStringLiteral(":limit"), xLimit);
    lQuery.bindValue(QStringLiteral(":offset"), xOffset);
    if (!lQuery.exec()) {
        return lRows;
    }

    while (lQuery.next()) {
        SignalNodeRecord lRow;
        lRow.dId = lQuery.value(0).toInt();
        lRow.dSignalName = lQuery.value(1).toString();
        lRow.dDirection = lQuery.value(2).toString();
        lRow.dWidthBits = lQuery.value(3).toInt();
        lRow.dDomainName = lQuery.value(4).toString();
        lRow.dChildCount = lQuery.value(5).toInt();
        lRows.append(lRow);
    }

    return lRows;
}
