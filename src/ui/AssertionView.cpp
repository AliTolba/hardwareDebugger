#include "AssertionView.h"

#include "BasicTableView.h"
#include "BasicTableViewModel.h"
#include "SelectionDetailsWidget.h"

#include <QAbstractItemModel>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHeaderView>

AssertionView::AssertionView(AppEventBus* xEventBus, QWidget* xParent)
    : QWidget(xParent)
    , dTableView(nullptr)
    , dFilterEdit(nullptr)
    , dDetailsWidget(nullptr)
    , dSplitter(nullptr)
    , dTableModel(nullptr)
{
    mCreateGui(xEventBus);
}

void AssertionView::mCreateGui(AppEventBus* xEventBus)
{
    dTableView = new BasicTableView(xEventBus, this);
    dFilterEdit = new QLineEdit(this);
    dDetailsWidget = new SelectionDetailsWidget(this);
    dSplitter = new QSplitter(Qt::Horizontal, this);

    auto* lLayout = new QVBoxLayout(this);
    lLayout->setContentsMargins(0, 0, 0, 0);

    auto* lFilterLayout = new QHBoxLayout();
    lFilterLayout->setContentsMargins(0, 0, 0, 0);
    lFilterLayout->addWidget(new QLabel(QStringLiteral("Filter:"), this));
    dFilterEdit->setPlaceholderText(QStringLiteral("Filter current view..."));
    lFilterLayout->addWidget(dFilterEdit, 1);

    dTableView->setSortingEnabled(true);

    connect(dFilterEdit, &QLineEdit::textChanged, this, &AssertionView::mApplyFilter);

    dSplitter->addWidget(dTableView);
    dSplitter->addWidget(dDetailsWidget);
    dSplitter->setStretchFactor(0, 4);
    dSplitter->setStretchFactor(1, 2);

    lLayout->addLayout(lFilterLayout);
    lLayout->addWidget(dSplitter);
}

void AssertionView::setModel(QAbstractItemModel* xModel)
{
    if (dSelectionConnection) {
        disconnect(dSelectionConnection);
    }
    if (dDataChangedConnection) {
        disconnect(dDataChangedConnection);
    }

    dTableModel = qobject_cast<BasicTableViewModel*>(xModel);
    dTableView->setModel(xModel);
    dDetailsWidget->mClear();

    if (dTableView->horizontalHeader() != nullptr && dTableModel != nullptr) {
        connect(
            dTableView->horizontalHeader(),
            &QHeaderView::sortIndicatorChanged,
            this,
            &AssertionView::mHandleColumnSorted);
    }

    if (dTableView->selectionModel() != nullptr) {
        dSelectionConnection = connect(
            dTableView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            [this](const QModelIndex& xCurrent, const QModelIndex&) { mUpdateDetails(xCurrent); });
    }

    if (xModel != nullptr) {
        dDataChangedConnection = connect(
            xModel,
            &QAbstractItemModel::dataChanged,
            this,
            [this](const QModelIndex&, const QModelIndex&, const QList<int>&) {
                mUpdateDetails(dTableView->currentIndex());
            });
    }
}

void AssertionView::mApplyFilter(const QString& xText)
{
    if (dTableModel == nullptr) {
        return;
    }

    if (xText.isEmpty()) {
        dTableModel->mClearFiltersAndSort();
    } else {
        dTableModel->mSetFilterPattern(xText);
    }

    mUpdateDetails(dTableView->currentIndex());
}

void AssertionView::mHandleColumnSorted(int xLogicalIndex, Qt::SortOrder xOrder)
{
    if (dTableModel == nullptr) {
        return;
    }

    dTableModel->mSetSortColumn(xLogicalIndex, xOrder);
    mUpdateDetails(dTableView->currentIndex());
}

void AssertionView::resizeColumnsToContents()
{
    dTableView->resizeColumnsToContents();
}

void AssertionView::mUpdateDetails(const QModelIndex& xCurrent)
{
    dDetailsWidget->mDisplayIndex(xCurrent);
}
