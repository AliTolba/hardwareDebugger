#include "AssertionViewService.h"

#include "AssertionRepository.h"

AssertionViewService::AssertionViewService(const AssertionRepository* xRepository)
    : dRepository(xRepository)
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
    return dRepository != nullptr ? dRepository->mTotalCount() : 0;
}

QVector<QVector<QVariant>> AssertionViewService::mFetchRows(int xOffset, int xLimit) const
{
    QVector<QVector<QVariant>> lRows;
    if (dRepository == nullptr) {
        return lRows;
    }

    const QVector<AssertionRecord> lRecords = dRepository->mFetchRows(xOffset, xLimit);
    lRows.reserve(lRecords.size());
    for (const AssertionRecord& lRecord : lRecords) {
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
