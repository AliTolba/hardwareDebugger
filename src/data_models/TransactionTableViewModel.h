#pragma once

#include "BasicTableViewModel.h"

class TransactionViewService;

class TransactionTableViewModel : public BasicTableViewModel
{
    Q_OBJECT

public:
    explicit TransactionTableViewModel(
        TransactionViewService* xService,
        int xBatchSize = 250,
        QObject* xParent = nullptr);

    Qt::ItemFlags flags(const QModelIndex& xIndex) const override;
    QVariant data(const QModelIndex& xIndex, int xRole = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& xIndex, const QVariant& xValue, int xRole = Qt::EditRole) override;

signals:
    void mTransactionUpdated();

private:
    static constexpr int sStatusColumn = 6;

    TransactionViewService* dTransactionService;
};
