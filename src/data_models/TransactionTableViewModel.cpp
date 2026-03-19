#include "TransactionTableViewModel.h"

#include "TransactionViewService.h"

TransactionTableViewModel::TransactionTableViewModel(
    TransactionViewService* xService,
    int xBatchSize,
    QObject* xParent)
    : BasicTableViewModel(xService, xBatchSize, xParent)
    , dTransactionService(xService)
{
}

Qt::ItemFlags TransactionTableViewModel::flags(const QModelIndex& xIndex) const
{
    Qt::ItemFlags lFlags = BasicTableViewModel::flags(xIndex);
    if (!xIndex.isValid()) {
        return lFlags;
    }

    if (xIndex.column() == sStatusColumn) {
        return lFlags | Qt::ItemIsEditable;
    }

    return lFlags;
}

QVariant TransactionTableViewModel::data(const QModelIndex& xIndex, int xRole) const
{
    if (xIndex.isValid()
        && xRole == Qt::EditRole
        && xIndex.row() >= 0
        && xIndex.row() < dRows.size()
        && xIndex.column() == sStatusColumn) {
        return dRows.at(xIndex.row()).at(xIndex.column());
    }

    return BasicTableViewModel::data(xIndex, xRole);
}

bool TransactionTableViewModel::setData(const QModelIndex& xIndex, const QVariant& xValue, int xRole)
{
    if (!xIndex.isValid() || xRole != Qt::EditRole || xIndex.column() != sStatusColumn) {
        return false;
    }

    if (xIndex.row() < 0 || xIndex.row() >= dRows.size() || dTransactionService == nullptr) {
        return false;
    }

    const QString lStatus = xValue.toString().trimmed();
    static const QStringList sAllowedStatuses = {
        QStringLiteral("Complete"),
        QStringLiteral("Pending"),
        QStringLiteral("Retry")
    };

    if (!sAllowedStatuses.contains(lStatus)) {
        return false;
    }

    if (dRows.at(xIndex.row()).at(xIndex.column()).toString() == lStatus) {
        return true;
    }

    if (!dTransactionService->mUpdateStatusAtRow(xIndex.row(), lStatus)) {
        return false;
    }

    dRows[xIndex.row()][xIndex.column()] = lStatus;
    emit dataChanged(xIndex, xIndex, {Qt::DisplayRole, Qt::EditRole});
    emit mTransactionUpdated();
    return true;
}
