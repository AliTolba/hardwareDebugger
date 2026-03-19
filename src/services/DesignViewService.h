#pragma once

#include "ViewDataServices.h"

class DesignRepository;

class DesignViewService : public TreeViewServiceInterface
{
public:
    explicit DesignViewService(const DesignRepository* xRepository = nullptr);

    QStringList mHeaders() const override;
    int mItemCount(const QVariant& xParentId) const override;
    QVector<TreeItemData> mFetchChildren(const QVariant& xParentId, int xOffset, int xLimit) const override;

private:
    const DesignRepository* dRepository;
};
