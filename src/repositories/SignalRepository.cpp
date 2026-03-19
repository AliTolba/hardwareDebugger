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

    QString lSql = QStringLiteral(
        "SELECT id, signal_name, direction, width_bits, domain_name, child_count "
        "FROM signal_nodes WHERE ");

    if (xParentId.isValid()) {
        lSql += QStringLiteral("parent_id = :parent_id");
    } else {
        lSql += QStringLiteral("parent_id IS NULL");
    }

    QString lWhereClause = mBuildWhereClause();
    if (!lWhereClause.isEmpty()) {
        lSql += QStringLiteral(" AND (") + lWhereClause + QStringLiteral(")");
    }

    lSql += QStringLiteral(" ") + mBuildOrderByClause();
    lSql += QStringLiteral(" LIMIT :limit OFFSET :offset");

    QSqlQuery lQuery(dDatabase->mConnection());
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

void SignalRepository::mSetFilterPattern(const QString& xPattern)
{
    dFilterPattern = xPattern;
}

void SignalRepository::mSetSortColumn(int xColumnIndex, Qt::SortOrder xOrder)
{
    dSortColumnIndex = xColumnIndex;
    dSortOrder = xOrder;
}

void SignalRepository::mClearFiltersAndSort()
{
    dFilterPattern.clear();
    dSortColumnIndex = -1;
    dSortOrder = Qt::AscendingOrder;
}

QString SignalRepository::mBuildWhereClause() const
{
    if (dFilterPattern.isEmpty()) {
        return QString();
    }

    const QStringList lColumns = {
        QStringLiteral("signal_name"),
        QStringLiteral("direction"),
        QStringLiteral("domain_name")
    };

    QStringList lConditions;
    for (const QString& lColumn : lColumns) {
        lConditions.append(lColumn + QStringLiteral(" LIKE '%") + dFilterPattern + QStringLiteral("%'"));
    }

    return lConditions.join(QStringLiteral(" OR "));
}

QString SignalRepository::mBuildOrderByClause() const
{
    const QStringList lColumns = {
        QStringLiteral("id"),
        QStringLiteral("signal_name"),
        QStringLiteral("direction"),
        QStringLiteral("width_bits"),
        QStringLiteral("domain_name")
    };

    if (dSortColumnIndex >= 0 && dSortColumnIndex < lColumns.size()) {
        const QString lColumnName = lColumns.at(dSortColumnIndex);
        const QString lOrderDir = (dSortOrder == Qt::AscendingOrder) ? QStringLiteral("ASC") : QStringLiteral("DESC");
        return QStringLiteral("ORDER BY ") + lColumnName + QStringLiteral(" ") + lOrderDir;
    }

    return QStringLiteral("ORDER BY id ASC");
}
