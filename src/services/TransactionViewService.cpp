#include "TransactionViewService.h"

#include "AppEventBus.h"
#include "BackgroundTaskService.h"
#include "DatabaseManager.h"
#include "TransactionRepository.h"

#include <QMetaObject>
#include <QPointer>
#include <QThread>

namespace
{
QVector<QVector<QVariant>> sBuildTransactionRows(const QVector<TransactionRecord>& xRecords)
{
    QVector<QVector<QVariant>> lRows;
    lRows.reserve(xRecords.size());
    for (const TransactionRecord& lRecord : xRecords) {
        lRows.append({
            lRecord.dChannel,
            lRecord.dOperation,
            QStringLiteral("0x%1").arg(lRecord.dAddress, 8, 16, QLatin1Char('0')).toUpper(),
            lRecord.dBurstLength,
            lRecord.dResponseCode,
            lRecord.dCycle,
            lRecord.dStatus
        });
    }

    return lRows;
}
}

TransactionViewService::TransactionViewService(const TransactionRepository* xRepository, AppEventBus* xEventBus)
    : dRepository(const_cast<TransactionRepository*>(xRepository))
    , dBackgroundTaskService(std::make_unique<BackgroundTaskService>(xEventBus))
{
}

QStringList TransactionViewService::mHeaders() const
{
    return {
        QStringLiteral("Channel"),
        QStringLiteral("Operation"),
        QStringLiteral("Address"),
        QStringLiteral("Burst"),
        QStringLiteral("Response"),
        QStringLiteral("Cycle"),
        QStringLiteral("Status")
    };
}

int TransactionViewService::mTotalRowCount() const
{
    if (dRepository == nullptr) {
        return 0;
    }

    dRepository->mSetFilterPattern(dFilterPattern);
    dRepository->mSetSortColumn(dSortColumnIndex, dSortOrder);
    return dRepository->mTotalCount();
}

QVector<QVector<QVariant>> TransactionViewService::mFetchRows(int xOffset, int xLimit) const
{
    if (dRepository == nullptr) {
        return {};
    }

    dRepository->mSetFilterPattern(dFilterPattern);
    dRepository->mSetSortColumn(dSortColumnIndex, dSortOrder);
    return sBuildTransactionRows(dRepository->mFetchRows(xOffset, xLimit));
}

bool TransactionViewService::mRequestTotalRowCount(QObject* xReceiver, const std::function<void(int)>& xOnLoaded)
{
    if (xReceiver == nullptr || !xOnLoaded || dBackgroundTaskService == nullptr) {
        return false;
    }

    const QString lFilterPattern = dFilterPattern;
    const int lSortColumnIndex = dSortColumnIndex;
    const Qt::SortOrder lSortOrder = dSortOrder;
    QPointer<QObject> lReceiver(xReceiver);

    return dBackgroundTaskService->mRunTask(QStringLiteral("Loading transaction row count"), [lReceiver, xOnLoaded, lFilterPattern, lSortColumnIndex, lSortOrder]() {
        int lTotalRowCount = 0;

        DatabaseManager lDatabase;
        if (lDatabase.mInitialize()) {
            TransactionRepository lRepository(&lDatabase);
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

bool TransactionViewService::mRequestRows(
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

    return dBackgroundTaskService->mRunTask(QStringLiteral("Loading transaction rows"), [lReceiver, xOnLoaded, lFilterPattern, lSortColumnIndex, lSortOrder, xOffset, xLimit]() {
        QVector<QVector<QVariant>> lRows;

        DatabaseManager lDatabase;
        if (lDatabase.mInitialize()) {
            TransactionRepository lRepository(&lDatabase);
            lRepository.mSetFilterPattern(lFilterPattern);
            lRepository.mSetSortColumn(lSortColumnIndex, lSortOrder);
            lRows = sBuildTransactionRows(lRepository.mFetchRows(xOffset, xLimit));
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

bool TransactionViewService::mUpdateStatusAtRow(int xRow, const QString& xStatus) const
{
    if (dRepository == nullptr) {
        return false;
    }

    return dRepository->mUpdateStatusAtRow(xRow, xStatus);
}

void TransactionViewService::mSetFilterPattern(const QString& xPattern)
{
    dFilterPattern = xPattern;
    if (dRepository != nullptr) {
        dRepository->mSetFilterPattern(xPattern);
    }
}

void TransactionViewService::mSetSortColumn(int xColumnIndex, Qt::SortOrder xOrder)
{
    dSortColumnIndex = xColumnIndex;
    dSortOrder = xOrder;
    if (dRepository != nullptr) {
        dRepository->mSetSortColumn(xColumnIndex, xOrder);
    }
}

void TransactionViewService::mClearFiltersAndSort()
{
    dFilterPattern.clear();
    dSortColumnIndex = -1;
    dSortOrder = Qt::AscendingOrder;
    if (dRepository != nullptr) {
        dRepository->mClearFiltersAndSort();
    }
}
