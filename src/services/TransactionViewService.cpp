#include "TransactionViewService.h"

#include "TransactionRepository.h"

TransactionViewService::TransactionViewService(const TransactionRepository* xRepository)
    : dRepository(xRepository)
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
    return dRepository != nullptr ? dRepository->mTotalCount() : 0;
}

QVector<QVector<QVariant>> TransactionViewService::mFetchRows(int xOffset, int xLimit) const
{
    QVector<QVector<QVariant>> lRows;
    if (dRepository == nullptr) {
        return lRows;
    }

    const QVector<TransactionRecord> lRecords = dRepository->mFetchRows(xOffset, xLimit);
    lRows.reserve(lRecords.size());
    for (const TransactionRecord& lRecord : lRecords) {
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

bool TransactionViewService::mUpdateStatusAtRow(int xRow, const QString& xStatus) const
{
    if (dRepository == nullptr) {
        return false;
    }

    return dRepository->mUpdateStatusAtRow(xRow, xStatus);
}
