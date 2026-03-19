#include "SignalViewService.h"

#include "SignalRepository.h"

SignalViewService::SignalViewService(const SignalRepository* xRepository)
    : dRepository(xRepository)
{
}

QStringList SignalViewService::mHeaders() const
{
    return {QStringLiteral("Signal"), QStringLiteral("Direction"), QStringLiteral("Width / Domain")};
}

int SignalViewService::mItemCount(const QVariant& xParentId) const
{
    return dRepository != nullptr ? dRepository->mItemCount(xParentId) : 0;
}

QVector<TreeItemData> SignalViewService::mFetchChildren(const QVariant& xParentId, int xOffset, int xLimit) const
{
    QVector<TreeItemData> lItems;
    if (dRepository == nullptr) {
        return lItems;
    }

    const QVector<SignalNodeRecord> lRecords = dRepository->mFetchChildren(xParentId, xOffset, xLimit);
    lItems.reserve(lRecords.size());
    for (const SignalNodeRecord& lRecord : lRecords) {
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
