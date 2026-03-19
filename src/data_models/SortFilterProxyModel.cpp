#include "SortFilterProxyModel.h"

#include <QModelIndex>
#include <QRegularExpression>

SortFilterProxyModel::SortFilterProxyModel(QObject* xParent)
    : QSortFilterProxyModel(xParent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setFilterKeyColumn(-1);
    setRecursiveFilteringEnabled(true);
    setDynamicSortFilter(true);
}

bool SortFilterProxyModel::filterAcceptsRow(int xSourceRow, const QModelIndex& xSourceParent) const
{
    if (filterRegularExpression().pattern().isEmpty()) {
        return true;
    }

    if (mRowMatchesAnyColumn(xSourceRow, xSourceParent)) {
        return true;
    }

    const QModelIndex lCurrent = sourceModel()->index(xSourceRow, 0, xSourceParent);
    if (!lCurrent.isValid()) {
        return false;
    }

    if (mAncestorMatches(lCurrent)) {
        return true;
    }

    return mAnyDescendantMatches(lCurrent);
}

bool SortFilterProxyModel::mRowMatchesAnyColumn(int xSourceRow, const QModelIndex& xSourceParent) const
{
    const QRegularExpression lRegex = filterRegularExpression();
    const int lColumnCount = sourceModel()->columnCount(xSourceParent);

    for (int lColumn = 0; lColumn < lColumnCount; ++lColumn) {
        const QModelIndex lIndex = sourceModel()->index(xSourceRow, lColumn, xSourceParent);
        const QString lText = sourceModel()->data(lIndex, Qt::DisplayRole).toString();
        if (lText.contains(lRegex)) {
            return true;
        }
    }

    return false;
}

bool SortFilterProxyModel::mAncestorMatches(const QModelIndex& xIndex) const
{
    QModelIndex lParent = xIndex.parent();
    while (lParent.isValid()) {
        if (mRowMatchesAnyColumn(lParent.row(), lParent.parent())) {
            return true;
        }
        lParent = lParent.parent();
    }

    return false;
}

bool SortFilterProxyModel::mAnyDescendantMatches(const QModelIndex& xParent) const
{
    const int lChildCount = sourceModel()->rowCount(xParent);
    for (int lChildRow = 0; lChildRow < lChildCount; ++lChildRow) {
        if (filterAcceptsRow(lChildRow, xParent)) {
            return true;
        }
    }

    return false;
}
