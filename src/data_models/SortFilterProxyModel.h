#pragma once

#include <QSortFilterProxyModel>

class SortFilterProxyModel : public QSortFilterProxyModel
{
public:
    explicit SortFilterProxyModel(QObject* xParent = nullptr);

protected:
    bool filterAcceptsRow(int xSourceRow, const QModelIndex& xSourceParent) const override;

private:
    bool mRowMatchesAnyColumn(int xSourceRow, const QModelIndex& xSourceParent) const;
    bool mAncestorMatches(const QModelIndex& xIndex) const;
    bool mAnyDescendantMatches(const QModelIndex& xParent) const;
};
