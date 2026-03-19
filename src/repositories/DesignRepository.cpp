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

    QSqlQuery lQuery(dDatabase->mConnection());
    QString lSql = QStringLiteral(
        "SELECT id, instance_name, node_kind, source_file, child_count "
        "FROM design_nodes ");
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
