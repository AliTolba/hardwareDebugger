#include "BasicTableViewModel.h"

#include "ViewDataServices.h"

BasicTableViewModel::BasicTableViewModel(
    TableViewServiceInterface* xService,
    int xBatchSize,
    QObject* xParent)
    : QAbstractTableModel(xParent)
    , dService(xService)
    , dHeaders(xService != nullptr ? xService->mHeaders() : QStringList())
    , dBatchSize(xBatchSize)
    , dTotalRows(xService != nullptr ? xService->mTotalRowCount() : 0)
{
    mLoadNextBatch(dBatchSize);
}

int BasicTableViewModel::rowCount(const QModelIndex& xParent) const
{
    return xParent.isValid() ? 0 : dRows.size();
}

int BasicTableViewModel::columnCount(const QModelIndex& xParent) const
{
    return xParent.isValid() ? 0 : dHeaders.size();
}

QVariant BasicTableViewModel::data(const QModelIndex& xIndex, int xRole) const
{
    if (!xIndex.isValid() || xRole != Qt::DisplayRole) {
        return QVariant();
    }

    if (xIndex.row() < 0 || xIndex.row() >= dRows.size() || xIndex.column() < 0 || xIndex.column() >= dHeaders.size()) {
        return QVariant();
    }

    return dRows.at(xIndex.row()).at(xIndex.column());
}

QVariant BasicTableViewModel::headerData(int xSection, Qt::Orientation xOrientation, int xRole) const
{
    if (xRole != Qt::DisplayRole) {
        return QVariant();
    }

    if (xOrientation == Qt::Horizontal && xSection >= 0 && xSection < dHeaders.size()) {
        return dHeaders.at(xSection);
    }

    if (xOrientation == Qt::Vertical) {
        return xSection + 1;
    }

    return QVariant();
}

bool BasicTableViewModel::canFetchMore(const QModelIndex& xParent) const
{
    return !xParent.isValid() && dRows.size() < dTotalRows;
}

void BasicTableViewModel::fetchMore(const QModelIndex& xParent)
{
    if (xParent.isValid()) {
        return;
    }

    mLoadNextBatch(dBatchSize);
}

void BasicTableViewModel::mResetCache()
{
    beginResetModel();
    dRows.clear();
    dHeaders = dService != nullptr ? dService->mHeaders() : QStringList();
    dTotalRows = dService != nullptr ? dService->mTotalRowCount() : 0;
    endResetModel();
    emit mLoadStateChanged(dRows.size(), dTotalRows);
    mLoadNextBatch(dBatchSize);
}

int BasicTableViewModel::mTotalRows() const
{
    return dTotalRows;
}

int BasicTableViewModel::mLoadedRows() const
{
    return dRows.size();
}

int BasicTableViewModel::mRemainingRows() const
{
    return dTotalRows - dRows.size();
}

int BasicTableViewModel::mRemainingBatches() const
{
    if (mRemainingRows() <= 0) {
        return 0;
    }

    return (mRemainingRows() + dBatchSize - 1) / dBatchSize;
}

void BasicTableViewModel::mLoadNextBatch(int xBatchSize)
{
    if (dService == nullptr || xBatchSize <= 0 || !canFetchMore(QModelIndex())) {
        emit mLoadStateChanged(dRows.size(), dTotalRows);
        return;
    }

    const int lStartRow = dRows.size();
    const int lLimit = qMin(xBatchSize, dTotalRows - lStartRow);
    const QVector<QVector<QVariant>> lNewRows = dService->mFetchRows(lStartRow, lLimit);
    if (lNewRows.isEmpty()) {
        emit mLoadStateChanged(dRows.size(), dTotalRows);
        return;
    }

    beginInsertRows(QModelIndex(), lStartRow, lStartRow + lNewRows.size() - 1);
    for (const QVector<QVariant>& lRow : lNewRows) {
        dRows.append(lRow);
    }
    endInsertRows();

    emit mLoadStateChanged(dRows.size(), dTotalRows);
}
