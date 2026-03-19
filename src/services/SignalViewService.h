#pragma once

#include "BackgroundTaskService.h"
#include "ViewDataServices.h"

#include <memory>

class SignalRepository;
class AppEventBus;

class SignalViewService : public TreeViewServiceInterface
{
public:
    explicit SignalViewService(const SignalRepository* xRepository = nullptr, AppEventBus* xEventBus = nullptr);

    QStringList mHeaders() const override;
    int mItemCount(const QVariant& xParentId) const override;
    QVector<TreeItemData> mFetchChildren(const QVariant& xParentId, int xOffset, int xLimit) const override;
    bool mRequestItemCount(
        const QVariant& xParentId,
        QObject* xReceiver,
        const std::function<void(int)>& xOnLoaded) override;
    bool mRequestChildren(
        const QVariant& xParentId,
        int xOffset,
        int xLimit,
        QObject* xReceiver,
        const std::function<void(QVector<TreeItemData>)>& xOnLoaded) override;

    void mSetFilterPattern(const QString& xPattern) override;
    void mSetSortColumn(int xColumnIndex, Qt::SortOrder xOrder) override;
    void mClearFiltersAndSort() override;

private:
    SignalRepository* dRepository;
    std::unique_ptr<BackgroundTaskService> dBackgroundTaskService;
    QString dFilterPattern;
    int dSortColumnIndex = -1;
    Qt::SortOrder dSortOrder = Qt::AscendingOrder;
};
