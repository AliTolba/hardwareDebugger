#pragma once

#include "ViewDataServices.h"

class TransactionRepository;

class TransactionViewService : public TableViewServiceInterface
{
public:
    explicit TransactionViewService(const TransactionRepository* xRepository = nullptr);

    QStringList mHeaders() const override;
    int mTotalRowCount() const override;
    QVector<QVector<QVariant>> mFetchRows(int xOffset, int xLimit) const override;
    bool mUpdateStatusAtRow(int xRow, const QString& xStatus) const;

private:
    const TransactionRepository* dRepository;
};
