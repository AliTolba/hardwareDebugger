#pragma once

#include "BackgroundTaskService.h"
#include "ViewDataServices.h"

#include <memory>

class AssertionRepository;
class AppEventBus;

class AssertionViewService : public TableViewServiceInterface
{
public:
    explicit AssertionViewService(const AssertionRepository* xRepository = nullptr, AppEventBus* xEventBus = nullptr);

    QStringList mHeaders() const override;
    int mTotalRowCount() const override;
    QVector<QVector<QVariant>> mFetchRows(int xOffset, int xLimit) const override;
    bool mRequestTotalRowCount(QObject* xReceiver, const std::function<void(int)>& xOnLoaded) override;
    bool mRequestRows(
        int xOffset,
        int xLimit,
        QObject* xReceiver,
        const std::function<void(QVector<QVector<QVariant>>)>& xOnLoaded) override;

    void mSetFilterPattern(const QString& xPattern) override;
    void mSetSortColumn(int xColumnIndex, Qt::SortOrder xOrder) override;
    void mClearFiltersAndSort() override;

private:
    AssertionRepository* dRepository;
    std::unique_ptr<BackgroundTaskService> dBackgroundTaskService;
    QString dFilterPattern;
    int dSortColumnIndex = -1;
    Qt::SortOrder dSortOrder = Qt::AscendingOrder;
};
