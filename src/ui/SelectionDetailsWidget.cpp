#include "SelectionDetailsWidget.h"

#include <QAbstractItemModel>
#include <QHeaderView>
#include <QLabel>
#include <QModelIndex>
#include <QTableWidget>
#include <QVBoxLayout>

SelectionDetailsWidget::SelectionDetailsWidget(QWidget* xParent)
    : QWidget(xParent)
    , dTitleLabel(nullptr)
    , dDetailsTable(nullptr)
{
    mCreateGui();
}

void SelectionDetailsWidget::mCreateGui()
{
    dTitleLabel = new QLabel(QStringLiteral("Select a row to view details."), this);
    dDetailsTable = new QTableWidget(this);

    auto* lLayout = new QVBoxLayout(this);
    lLayout->setContentsMargins(0, 0, 0, 0);
    lLayout->setSpacing(8);

    dDetailsTable->setColumnCount(2);
    dDetailsTable->setHorizontalHeaderLabels({QStringLiteral("Field"), QStringLiteral("Value")});
    dDetailsTable->horizontalHeader()->setStretchLastSection(true);
    dDetailsTable->verticalHeader()->setVisible(false);
    dDetailsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    dDetailsTable->setSelectionMode(QAbstractItemView::NoSelection);
    dDetailsTable->setAlternatingRowColors(true);

    lLayout->addWidget(dTitleLabel);
    lLayout->addWidget(dDetailsTable, 1);
}

void SelectionDetailsWidget::mDisplayIndex(const QModelIndex& xIndex)
{
    if (!xIndex.isValid() || xIndex.model() == nullptr) {
        mClear();
        return;
    }

    dTitleLabel->setText(QStringLiteral("Selected row details"));

    const QAbstractItemModel* lModel = xIndex.model();
    const int lColumnCount = lModel->columnCount(xIndex.parent());
    dDetailsTable->setRowCount(lColumnCount);

    for (int lColumn = 0; lColumn < lColumnCount; ++lColumn) {
        const QString lHeader = lModel->headerData(lColumn, Qt::Horizontal, Qt::DisplayRole).toString();
        const QString lValue = lModel->data(lModel->index(xIndex.row(), lColumn, xIndex.parent()), Qt::DisplayRole).toString();

        auto* lHeaderItem = new QTableWidgetItem(lHeader);
        lHeaderItem->setFlags(lHeaderItem->flags() & ~Qt::ItemIsEditable);
        auto* lValueItem = new QTableWidgetItem(lValue);
        lValueItem->setFlags(lValueItem->flags() & ~Qt::ItemIsEditable);

        dDetailsTable->setItem(lColumn, 0, lHeaderItem);
        dDetailsTable->setItem(lColumn, 1, lValueItem);
    }

    dDetailsTable->resizeRowsToContents();
}

void SelectionDetailsWidget::mClear()
{
    dTitleLabel->setText(QStringLiteral("Select a row to view details."));
    dDetailsTable->clearContents();
    dDetailsTable->setRowCount(0);
}
