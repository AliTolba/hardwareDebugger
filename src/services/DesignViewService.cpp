#include "DesignViewService.h"

#include "AppEventBus.h"
#include "BackgroundTaskService.h"
#include "DatabaseManager.h"
#include "DesignRepository.h"

#include <QMetaObject>
#include <QPointer>

namespace
{
QVector<TreeItemData> sBuildDesignItems(const QVector<DesignNodeRecord>& xRecords)
{
    QVector<TreeItemData> lItems;
    lItems.reserve(xRecords.size());
    for (const DesignNodeRecord& lRecord : xRecords) {
        TreeItemData lItem;
        lItem.dId = lRecord.dId;
        lItem.dColumns = {lRecord.dInstanceName, lRecord.dNodeKind, lRecord.dSourceFile};
        lItem.dChildCount = lRecord.dChildCount;
        lItems.append(lItem);
    }

    return lItems;
}
}

DesignViewService::DesignViewService(const DesignRepository* xRepository, AppEventBus* xEventBus)
    : dRepository(const_cast<DesignRepository*>(xRepository))
    , dBackgroundTaskService(std::make_unique<BackgroundTaskService>(xEventBus))
{
}

QStringList DesignViewService::mHeaders() const
{
    return {QStringLiteral("Instance"), QStringLiteral("Node Type"), QStringLiteral("Source File")};
}

int DesignViewService::mItemCount(const QVariant& xParentId) const
{
    if (dRepository == nullptr) {
        return 0;
    }

    dRepository->mSetFilterPattern(dFilterPattern);
    dRepository->mSetSortColumn(dSortColumnIndex, dSortOrder);
    return dRepository->mItemCount(xParentId);
}

QVector<TreeItemData> DesignViewService::mFetchChildren(const QVariant& xParentId, int xOffset, int xLimit) const
{
    if (dRepository == nullptr) {
        return {};
    }

    dRepository->mSetFilterPattern(dFilterPattern);
    dRepository->mSetSortColumn(dSortColumnIndex, dSortOrder);
    return sBuildDesignItems(dRepository->mFetchChildren(xParentId, xOffset, xLimit));
}

bool DesignViewService::mRequestItemCount(
    const QVariant& xParentId,
    QObject* xReceiver,
    const std::function<void(int)>& xOnLoaded)
{
    if (xReceiver == nullptr || !xOnLoaded || dBackgroundTaskService == nullptr) {
        return false;
    }

    const QString lFilterPattern = dFilterPattern;
    const int lSortColumnIndex = dSortColumnIndex;
    const Qt::SortOrder lSortOrder = dSortOrder;
    QPointer<QObject> lReceiver(xReceiver);

    return dBackgroundTaskService->mRunTask(QStringLiteral("Loading design item count"), [lReceiver, xOnLoaded, lFilterPattern, lSortColumnIndex, lSortOrder, xParentId]() {
        int lItemCount = 0;

        DatabaseManager lDatabase;
        if (lDatabase.mInitialize()) {
            DesignRepository lRepository(&lDatabase);
            lRepository.mSetFilterPattern(lFilterPattern);
            lRepository.mSetSortColumn(lSortColumnIndex, lSortOrder);
            lItemCount = lRepository.mItemCount(xParentId);
        }

        if (lReceiver != nullptr) {
            QMetaObject::invokeMethod(lReceiver, [lReceiver, xOnLoaded, lItemCount]() {
                if (lReceiver != nullptr) {
                    xOnLoaded(lItemCount);
                }
            }, Qt::QueuedConnection);
        }
    });
}

bool DesignViewService::mRequestChildren(
    const QVariant& xParentId,
    int xOffset,
    int xLimit,
    QObject* xReceiver,
    const std::function<void(QVector<TreeItemData>)>& xOnLoaded)
{
    if (xReceiver == nullptr || !xOnLoaded || dBackgroundTaskService == nullptr) {
        return false;
    }

    const QString lFilterPattern = dFilterPattern;
    const int lSortColumnIndex = dSortColumnIndex;
    const Qt::SortOrder lSortOrder = dSortOrder;
    QPointer<QObject> lReceiver(xReceiver);

    return dBackgroundTaskService->mRunTask(QStringLiteral("Loading design children"), [lReceiver, xOnLoaded, lFilterPattern, lSortColumnIndex, lSortOrder, xParentId, xOffset, xLimit]() {
        QVector<TreeItemData> lItems;

        DatabaseManager lDatabase;
        if (lDatabase.mInitialize()) {
            DesignRepository lRepository(&lDatabase);
            lRepository.mSetFilterPattern(lFilterPattern);
            lRepository.mSetSortColumn(lSortColumnIndex, lSortOrder);
            lItems = sBuildDesignItems(lRepository.mFetchChildren(xParentId, xOffset, xLimit));
        }

        if (lReceiver != nullptr) {
            QMetaObject::invokeMethod(lReceiver, [lReceiver, xOnLoaded, lItems]() mutable {
                if (lReceiver != nullptr) {
                    xOnLoaded(std::move(lItems));
                }
            }, Qt::QueuedConnection);
        }
    });
}

void DesignViewService::mSetFilterPattern(const QString& xPattern)
{
    dFilterPattern = xPattern;
    if (dRepository != nullptr) {
        dRepository->mSetFilterPattern(xPattern);
    }
}

void DesignViewService::mSetSortColumn(int xColumnIndex, Qt::SortOrder xOrder)
{
    dSortColumnIndex = xColumnIndex;
    dSortOrder = xOrder;
    if (dRepository != nullptr) {
        dRepository->mSetSortColumn(xColumnIndex, xOrder);
    }
}

void DesignViewService::mClearFiltersAndSort()
{
    dFilterPattern.clear();
    dSortColumnIndex = -1;
    dSortOrder = Qt::AscendingOrder;
    if (dRepository != nullptr) {
        dRepository->mClearFiltersAndSort();
    }
}
