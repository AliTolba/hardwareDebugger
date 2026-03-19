#pragma once

#include <QAbstractTableModel>
#include <QPointer>
#include <QStringList>
#include <QVector>

class TableViewServiceInterface;

class BasicTableViewModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit BasicTableViewModel(
        TableViewServiceInterface* xService,
        int xBatchSize = 250,
        QObject* xParent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    void mResetCache();
    void mFetchAllRemaining();
    int mTotalRows() const;
    int mLoadedRows() const;
    int mRemainingRows() const;
    int mRemainingBatches() const;

    void mSetFilterPattern(const QString& xPattern);
    void mSetSortColumn(int xColumnIndex, Qt::SortOrder xOrder);
    void mClearFiltersAndSort();

signals:
    void mLoadStateChanged(int xLoadedRows, int xTotalRows);

protected:
    void mLoadNextBatch(int xBatchSize);
    void mRequestTotalRowCount(bool xAutoFetchFirstBatch);

    TableViewServiceInterface* dService;
    QStringList dHeaders;
    int dBatchSize;
    int dTotalRows;
    QVector<QVector<QVariant>> dRows;
    bool dIsLoading;
    bool dFetchAllRequested;
    int dGeneration;
};
