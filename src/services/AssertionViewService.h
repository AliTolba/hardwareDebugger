#pragma once

#include "ViewDataServices.h"

class AssertionRepository;

class AssertionViewService : public TableViewServiceInterface
{
public:
    explicit AssertionViewService(const AssertionRepository* xRepository = nullptr);

    QStringList mHeaders() const override;
    int mTotalRowCount() const override;
    QVector<QVector<QVariant>> mFetchRows(int xOffset, int xLimit) const override;

private:
    const AssertionRepository* dRepository;
};
