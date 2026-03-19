#include "SignalViewService.h"

#include "AppEventBus.h"
#include "BackgroundTaskService.h"
#include "DatabaseManager.h"
#include "SignalRepository.h"

#include <QMetaObject>
#include <QPointer>

namespace
{
QVector<TreeItemData> sBuildSignalItems(const QVector<SignalNodeRecord>& xRecords)
{
    QVector<TreeItemData> lItems;
    lItems.reserve(xRecords.size());
    for (const SignalNodeRecord& lRecord : xRecords) {
        TreeItemData lItem;
        lItem.dId = lRecord.dId;
        lItem.dColumns = {
            lRecord.dSignalName,
            lRecord.dDirection,
            QStringLiteral("%1 bits / %2").arg(lRecord.dWidthBits).arg(lRecord.dDomainName)
        };
        lItem.dChildCount = lRecord.dChildCount;
        lItems.append(lItem);
    }

    return lItems;
}
}

SignalViewService::SignalViewService(const SignalRepository* xRepository, AppEventBus* xEventBus)
    : dRepository(const_cast<SignalRepository*>(xRepository))
    , dBackgroundTaskService(std::make_unique<BackgroundTaskService>(xEventBus))
{
}

QStringList SignalViewService::mHeaders() const
{
    return {QStringLiteral("Signal"), QStringLiteral("Direction"), QStringLiteral("Width / Domain")};
}

int SignalViewService::mItemCount(const QVariant& xParentId) const
{
    if (dRepository == nullptr) {
        return 0;
    }

    dRepository->mSetFilterPattern(dFilterPattern);
    dRepository->mSetSortColumn(dSortColumnIndex, dSortOrder);
    return dRepository->mItemCount(xParentId);
}

QVector<TreeItemData> SignalViewService::mFetchChildren(const QVariant& xParentId, int xOffset, int xLimit) const
{
    if (dRepository == nullptr) {
        return {};
    }

    dRepository->mSetFilterPattern(dFilterPattern);
    dRepository->mSetSortColumn(dSortColumnIndex, dSortOrder);
    return sBuildSignalItems(dRepository->mFetchChildren(xParentId, xOffset, xLimit));
}

bool SignalViewService::mRequestItemCount(
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

    return dBackgroundTaskService->mRunTask(QStringLiteral("Loading signal item count"), [lReceiver, xOnLoaded, lFilterPattern, lSortColumnIndex, lSortOrder, xParentId]() {
        int lItemCount = 0;

        DatabaseManager lDatabase;
        if (lDatabase.mInitialize()) {
            SignalRepository lRepository(&lDatabase);
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

bool SignalViewService::mRequestChildren(
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

    return dBackgroundTaskService->mRunTask(QStringLiteral("Loading signal children"), [lReceiver, xOnLoaded, lFilterPattern, lSortColumnIndex, lSortOrder, xParentId, xOffset, xLimit]() {
        QVector<TreeItemData> lItems;

        DatabaseManager lDatabase;
        if (lDatabase.mInitialize()) {
            SignalRepository lRepository(&lDatabase);
            lRepository.mSetFilterPattern(lFilterPattern);
            lRepository.mSetSortColumn(lSortColumnIndex, lSortOrder);
            lItems = sBuildSignalItems(lRepository.mFetchChildren(xParentId, xOffset, xLimit));
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

void SignalViewService::mSetFilterPattern(const QString& xPattern)
{
    dFilterPattern = xPattern;
    if (dRepository != nullptr) {
        dRepository->mSetFilterPattern(xPattern);
    }
}

void SignalViewService::mSetSortColumn(int xColumnIndex, Qt::SortOrder xOrder)
{
    dSortColumnIndex = xColumnIndex;
    dSortOrder = xOrder;
    if (dRepository != nullptr) {
        dRepository->mSetSortColumn(xColumnIndex, xOrder);
    }
}

void SignalViewService::mClearFiltersAndSort()
{
    dFilterPattern.clear();
    dSortColumnIndex = -1;
    dSortOrder = Qt::AscendingOrder;
    if (dRepository != nullptr) {
        dRepository->mClearFiltersAndSort();
    }
}
