#include "DesignViewService.h"

#include "DesignRepository.h"

DesignViewService::DesignViewService(const DesignRepository* xRepository)
    : dRepository(xRepository)
{
}

QStringList DesignViewService::mHeaders() const
{
    return {QStringLiteral("Instance"), QStringLiteral("Node Type"), QStringLiteral("Source File")};
}

int DesignViewService::mItemCount(const QVariant& xParentId) const
{
    return dRepository != nullptr ? dRepository->mItemCount(xParentId) : 0;
}

QVector<TreeItemData> DesignViewService::mFetchChildren(const QVariant& xParentId, int xOffset, int xLimit) const
{
    QVector<TreeItemData> lItems;
    if (dRepository == nullptr) {
        return lItems;
    }

    const QVector<DesignNodeRecord> lRecords = dRepository->mFetchChildren(xParentId, xOffset, xLimit);
    lItems.reserve(lRecords.size());
    for (const DesignNodeRecord& lRecord : lRecords) {
        TreeItemData lItem;
        lItem.dId = lRecord.dId;
        lItem.dColumns = {lRecord.dInstanceName, lRecord.dNodeKind, lRecord.dSourceFile};
        lItem.dChildCount = lRecord.dChildCount;
        lItems.append(lItem);
    }

    return lItems;
}
