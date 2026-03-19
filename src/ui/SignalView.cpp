#include "SignalView.h"

#include "BasicTreeView.h"
#include "BasicTreeViewModel.h"
#include "SelectionDetailsWidget.h"

#include <QAbstractItemModel>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QSplitter>
#include <QVBoxLayout>

SignalView::SignalView(AppEventBus* xEventBus, QWidget* xParent)
    : QWidget(xParent)
    , dTreeView(nullptr)
    , dFilterEdit(nullptr)
    , dDetailsWidget(nullptr)
    , dSplitter(nullptr)
    , dTreeModel(nullptr)
{
    mCreateGui(xEventBus);
}

void SignalView::mCreateGui(AppEventBus* xEventBus)
{
    dTreeView = new BasicTreeView(xEventBus, this);
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

    dTreeView->setSortingEnabled(true);

    connect(dFilterEdit, &QLineEdit::textChanged, this, &SignalView::mApplyFilter);

    dSplitter->addWidget(dTreeView);
    dSplitter->addWidget(dDetailsWidget);
    dSplitter->setStretchFactor(0, 4);
    dSplitter->setStretchFactor(1, 2);

    lLayout->addLayout(lFilterLayout);
    lLayout->addWidget(dSplitter);
}

void SignalView::setModel(QAbstractItemModel* xModel)
{
    if (dSelectionConnection) {
        disconnect(dSelectionConnection);
    }
    if (dDataChangedConnection) {
        disconnect(dDataChangedConnection);
    }

    dTreeModel = qobject_cast<BasicTreeViewModel*>(xModel);
    dTreeView->setModel(xModel);
    dDetailsWidget->mClear();

    if (dTreeView->header() != nullptr && dTreeModel != nullptr) {
        connect(
            dTreeView->header(),
            &QHeaderView::sortIndicatorChanged,
            this,
            &SignalView::mHandleColumnSorted);
    }

    if (dTreeView->selectionModel() != nullptr) {
        dSelectionConnection = connect(
            dTreeView->selectionModel(),
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
                mUpdateDetails(dTreeView->currentIndex());
            });
    }
}

void SignalView::mApplyFilter(const QString& xText)
{
    if (dTreeModel == nullptr) {
        return;
    }

    if (xText.isEmpty()) {
        dTreeModel->mClearFiltersAndSort();
    } else {
        dTreeModel->mSetFilterPattern(xText);
    }

    mUpdateDetails(dTreeView->currentIndex());
}

void SignalView::mHandleColumnSorted(int xLogicalIndex, Qt::SortOrder xOrder)
{
    if (dTreeModel == nullptr) {
        return;
    }

    dTreeModel->mSetSortColumn(xLogicalIndex, xOrder);
    mUpdateDetails(dTreeView->currentIndex());
}

QHeaderView* SignalView::header() const
{
    return dTreeView->header();
}

void SignalView::mUpdateDetails(const QModelIndex& xCurrent)
{
    dDetailsWidget->mDisplayIndex(xCurrent);
}
