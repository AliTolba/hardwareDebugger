#include "DesignRepository.h"

#include "DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlQuery>

DesignRepository::DesignRepository(DatabaseManager* xDatabase)
    : dDatabase(xDatabase)
{
}

int DesignRepository::mTotalCount() const
{
    if (dDatabase == nullptr) {
        return 0;
    }

    QSqlQuery lQuery(dDatabase->mConnection());
    lQuery.exec(QStringLiteral("SELECT COUNT(*) FROM design_nodes"));
    return lQuery.next() ? lQuery.value(0).toInt() : 0;
}

int DesignRepository::mItemCount(const QVariant& xParentId) const
{
    if (dDatabase == nullptr) {
        return 0;
    }

    QSqlQuery lQuery(dDatabase->mConnection());
    QString lSql = QStringLiteral("SELECT COUNT(*) FROM design_nodes ");
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

QVector<DesignNodeRecord> DesignRepository::mFetchChildren(const QVariant& xParentId, int xOffset, int xLimit) const
{
    QVector<DesignNodeRecord> lRows;
    if (dDatabase == nullptr) {
        return lRows;
    }

    QString lSql = QStringLiteral(
        "SELECT id, instance_name, node_kind, source_file, child_count "
        "FROM design_nodes WHERE ");

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
        DesignNodeRecord lRow;
        lRow.dId = lQuery.value(0).toInt();
        lRow.dInstanceName = lQuery.value(1).toString();
        lRow.dNodeKind = lQuery.value(2).toString();
        lRow.dSourceFile = lQuery.value(3).toString();
        lRow.dChildCount = lQuery.value(4).toInt();
        lRows.append(lRow);
    }

    return lRows;
}

void DesignRepository::mSetFilterPattern(const QString& xPattern)
{
    dFilterPattern = xPattern;
}

void DesignRepository::mSetSortColumn(int xColumnIndex, Qt::SortOrder xOrder)
{
    dSortColumnIndex = xColumnIndex;
    dSortOrder = xOrder;
}

void DesignRepository::mClearFiltersAndSort()
{
    dFilterPattern.clear();
    dSortColumnIndex = -1;
    dSortOrder = Qt::AscendingOrder;
}

QString DesignRepository::mBuildWhereClause() const
{
    if (dFilterPattern.isEmpty()) {
        return QString();
    }

    const QStringList lColumns = {
        QStringLiteral("instance_name"),
        QStringLiteral("node_kind"),
        QStringLiteral("source_file")
    };

    QStringList lConditions;
    for (const QString& lColumn : lColumns) {
        lConditions.append(lColumn + QStringLiteral(" LIKE '%") + dFilterPattern + QStringLiteral("%'"));
    }

    return lConditions.join(QStringLiteral(" OR "));
}

QString DesignRepository::mBuildOrderByClause() const
{
    const QStringList lColumns = {
        QStringLiteral("id"),
        QStringLiteral("instance_name"),
        QStringLiteral("node_kind"),
        QStringLiteral("source_file")
    };

    if (dSortColumnIndex >= 0 && dSortColumnIndex < lColumns.size()) {
        const QString lColumnName = lColumns.at(dSortColumnIndex);
        const QString lOrderDir = (dSortOrder == Qt::AscendingOrder) ? QStringLiteral("ASC") : QStringLiteral("DESC");
        return QStringLiteral("ORDER BY ") + lColumnName + QStringLiteral(" ") + lOrderDir;
    }

    return QStringLiteral("ORDER BY id ASC");
}
