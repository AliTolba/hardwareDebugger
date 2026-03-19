#include "AssertionViewService.h"

#include "AppEventBus.h"
#include "AssertionRepository.h"
#include "BackgroundTaskService.h"
#include "DatabaseManager.h"

#include <QMetaObject>
#include <QPointer>

namespace
{
QVector<QVector<QVariant>> sBuildAssertionRows(const QVector<AssertionRecord>& xRecords)
{
    QVector<QVector<QVariant>> lRows;
    lRows.reserve(xRecords.size());
    for (const AssertionRecord& lRecord : xRecords) {
        lRows.append({
            lRecord.dPropertyName,
            lRecord.dModuleName,
            lRecord.dSeverity,
            lRecord.dStatus,
            lRecord.dMessage,
            lRecord.dCycle,
            lRecord.dTimeNs
        });
    }

    return lRows;
}
}

AssertionViewService::AssertionViewService(const AssertionRepository* xRepository, AppEventBus* xEventBus)
    : dRepository(const_cast<AssertionRepository*>(xRepository))
    , dBackgroundTaskService(std::make_unique<BackgroundTaskService>(xEventBus))
{
}

QStringList AssertionViewService::mHeaders() const
{
    return {
        QStringLiteral("Property"),
        QStringLiteral("Module"),
        QStringLiteral("Severity"),
        QStringLiteral("Status"),
        QStringLiteral("Message"),
        QStringLiteral("Cycle"),
        QStringLiteral("Time (ns)")
    };
}

int AssertionViewService::mTotalRowCount() const
{
    if (dRepository == nullptr) {
        return 0;
    }

    dRepository->mSetFilterPattern(dFilterPattern);
    dRepository->mSetSortColumn(dSortColumnIndex, dSortOrder);
    return dRepository->mTotalCount();
}

QVector<QVector<QVariant>> AssertionViewService::mFetchRows(int xOffset, int xLimit) const
{
    if (dRepository == nullptr) {
        return {};
    }

    dRepository->mSetFilterPattern(dFilterPattern);
    dRepository->mSetSortColumn(dSortColumnIndex, dSortOrder);
    return sBuildAssertionRows(dRepository->mFetchRows(xOffset, xLimit));
}

bool AssertionViewService::mRequestTotalRowCount(QObject* xReceiver, const std::function<void(int)>& xOnLoaded)
{
    if (xReceiver == nullptr || !xOnLoaded || dBackgroundTaskService == nullptr) {
        return false;
    }

    const QString lFilterPattern = dFilterPattern;
    const int lSortColumnIndex = dSortColumnIndex;
    const Qt::SortOrder lSortOrder = dSortOrder;
    QPointer<QObject> lReceiver(xReceiver);

    return dBackgroundTaskService->mRunTask(QStringLiteral("Loading assertion row count"), [lReceiver, xOnLoaded, lFilterPattern, lSortColumnIndex, lSortOrder]() {
        int lTotalRowCount = 0;

        DatabaseManager lDatabase;
        if (lDatabase.mInitialize()) {
            AssertionRepository lRepository(&lDatabase);
            lRepository.mSetFilterPattern(lFilterPattern);
            lRepository.mSetSortColumn(lSortColumnIndex, lSortOrder);
            lTotalRowCount = lRepository.mTotalCount();
        }

        if (lReceiver != nullptr) {
            QMetaObject::invokeMethod(lReceiver, [lReceiver, xOnLoaded, lTotalRowCount]() {
                if (lReceiver != nullptr) {
                    xOnLoaded(lTotalRowCount);
                }
            }, Qt::QueuedConnection);
        }
    });
}

bool AssertionViewService::mRequestRows(
    int xOffset,
    int xLimit,
    QObject* xReceiver,
    const std::function<void(QVector<QVector<QVariant>>)>& xOnLoaded)
{
    if (xReceiver == nullptr || !xOnLoaded || dBackgroundTaskService == nullptr) {
        return false;
    }

    const QString lFilterPattern = dFilterPattern;
    const int lSortColumnIndex = dSortColumnIndex;
    const Qt::SortOrder lSortOrder = dSortOrder;
    QPointer<QObject> lReceiver(xReceiver);

    return dBackgroundTaskService->mRunTask(QStringLiteral("Loading assertion rows"), [lReceiver, xOnLoaded, lFilterPattern, lSortColumnIndex, lSortOrder, xOffset, xLimit]() {
        QVector<QVector<QVariant>> lRows;

        DatabaseManager lDatabase;
        if (lDatabase.mInitialize()) {
            AssertionRepository lRepository(&lDatabase);
            lRepository.mSetFilterPattern(lFilterPattern);
            lRepository.mSetSortColumn(lSortColumnIndex, lSortOrder);
            lRows = sBuildAssertionRows(lRepository.mFetchRows(xOffset, xLimit));
        }

        if (lReceiver != nullptr) {
            QMetaObject::invokeMethod(lReceiver, [lReceiver, xOnLoaded, lRows]() mutable {
                if (lReceiver != nullptr) {
                    xOnLoaded(std::move(lRows));
                }
            }, Qt::QueuedConnection);
        }
    });
}

void AssertionViewService::mSetFilterPattern(const QString& xPattern)
{
    dFilterPattern = xPattern;
    if (dRepository != nullptr) {
        dRepository->mSetFilterPattern(xPattern);
    }
}

void AssertionViewService::mSetSortColumn(int xColumnIndex, Qt::SortOrder xOrder)
{
    dSortColumnIndex = xColumnIndex;
    dSortOrder = xOrder;
    if (dRepository != nullptr) {
        dRepository->mSetSortColumn(xColumnIndex, xOrder);
    }
}

void AssertionViewService::mClearFiltersAndSort()
{
    dFilterPattern.clear();
    dSortColumnIndex = -1;
    dSortOrder = Qt::AscendingOrder;
    if (dRepository != nullptr) {
        dRepository->mClearFiltersAndSort();
    }
}
