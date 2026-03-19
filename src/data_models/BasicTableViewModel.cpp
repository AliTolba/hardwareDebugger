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
    , dTotalRows(0)
    , dIsLoading(false)
    , dFetchAllRequested(false)
    , dGeneration(0)
{
    mRequestTotalRowCount(true);
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
    ++dGeneration;
    dRows.clear();
    dHeaders = dService != nullptr ? dService->mHeaders() : QStringList();
    dTotalRows = 0;
    dIsLoading = false;
    dFetchAllRequested = false;
    endResetModel();
    emit mLoadStateChanged(dRows.size(), dTotalRows);
    mRequestTotalRowCount(true);
}

void BasicTableViewModel::mFetchAllRemaining()
{
    if (!canFetchMore(QModelIndex())) {
        return;
    }

    dFetchAllRequested = true;
    if (!dIsLoading) {
        mLoadNextBatch(dBatchSize);
    }
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
    if (dService == nullptr || xBatchSize <= 0 || !canFetchMore(QModelIndex()) || dIsLoading) {
        emit mLoadStateChanged(dRows.size(), dTotalRows);
        return;
    }

    const int lStartRow = dRows.size();
    const int lLimit = qMin(xBatchSize, dTotalRows - lStartRow);
    const int lGeneration = dGeneration;
    dIsLoading = true;

    if (!dService->mRequestRows(lStartRow, lLimit, this, [this, lGeneration, lStartRow](QVector<QVector<QVariant>> xNewRows) {
        if (lGeneration != dGeneration) {
            return;
        }

        dIsLoading = false;
        if (!xNewRows.isEmpty()) {
            beginInsertRows(QModelIndex(), lStartRow, lStartRow + xNewRows.size() - 1);
            for (const QVector<QVariant>& lRow : xNewRows) {
                dRows.append(lRow);
            }
            endInsertRows();
        }

        emit mLoadStateChanged(dRows.size(), dTotalRows);

        if (dFetchAllRequested && canFetchMore(QModelIndex())) {
            mLoadNextBatch(dBatchSize);
            return;
        }

        dFetchAllRequested = false;
    })) {
        dIsLoading = false;
        emit mLoadStateChanged(dRows.size(), dTotalRows);
    }
}

void BasicTableViewModel::mRequestTotalRowCount(bool xAutoFetchFirstBatch)
{
    if (dService == nullptr || dIsLoading) {
        return;
    }

    const int lGeneration = dGeneration;
    dIsLoading = true;

    if (!dService->mRequestTotalRowCount(this, [this, lGeneration, xAutoFetchFirstBatch](int xTotalRows) {
        if (lGeneration != dGeneration) {
            return;
        }

        dTotalRows = xTotalRows;
        dIsLoading = false;
        emit mLoadStateChanged(dRows.size(), dTotalRows);

        if (xAutoFetchFirstBatch && canFetchMore(QModelIndex())) {
            mLoadNextBatch(dBatchSize);
        }
    })) {
        dIsLoading = false;
        emit mLoadStateChanged(dRows.size(), dTotalRows);
    }
}

void BasicTableViewModel::mSetFilterPattern(const QString& xPattern)
{
    if (dService != nullptr) {
        dService->mSetFilterPattern(xPattern);
        mResetCache();
    }
}

void BasicTableViewModel::mSetSortColumn(int xColumnIndex, Qt::SortOrder xOrder)
{
    if (dService != nullptr) {
        dService->mSetSortColumn(xColumnIndex, xOrder);
        mResetCache();
    }
}

void BasicTableViewModel::mClearFiltersAndSort()
{
    if (dService != nullptr) {
        dService->mClearFiltersAndSort();
        mResetCache();
    }
}
