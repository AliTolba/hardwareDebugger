#pragma once

#include "BackgroundTaskService.h"
#include "ViewDataServices.h"

#include <memory>

class TransactionRepository;
class AppEventBus;

class TransactionViewService : public TableViewServiceInterface
{
public:
    explicit TransactionViewService(const TransactionRepository* xRepository = nullptr, AppEventBus* xEventBus = nullptr);

    QStringList mHeaders() const override;
    int mTotalRowCount() const override;
    QVector<QVector<QVariant>> mFetchRows(int xOffset, int xLimit) const override;
    bool mRequestTotalRowCount(QObject* xReceiver, const std::function<void(int)>& xOnLoaded) override;
    bool mRequestRows(
        int xOffset,
        int xLimit,
        QObject* xReceiver,
        const std::function<void(QVector<QVector<QVariant>>)>& xOnLoaded) override;
    bool mUpdateStatusAtRow(int xRow, const QString& xStatus) const;

    void mSetFilterPattern(const QString& xPattern) override;
    void mSetSortColumn(int xColumnIndex, Qt::SortOrder xOrder) override;
    void mClearFiltersAndSort() override;

private:
    TransactionRepository* dRepository;
    std::unique_ptr<BackgroundTaskService> dBackgroundTaskService;
    QString dFilterPattern;
    int dSortColumnIndex = -1;
    Qt::SortOrder dSortOrder = Qt::AscendingOrder;
};
