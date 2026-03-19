#pragma once

#include "ViewDataServices.h"

class SignalRepository;

class SignalViewService : public TreeViewServiceInterface
{
public:
    explicit SignalViewService(const SignalRepository* xRepository = nullptr);

    QStringList mHeaders() const override;
    int mItemCount(const QVariant& xParentId) const override;
    QVector<TreeItemData> mFetchChildren(const QVariant& xParentId, int xOffset, int xLimit) const override;

private:
    const SignalRepository* dRepository;
};
