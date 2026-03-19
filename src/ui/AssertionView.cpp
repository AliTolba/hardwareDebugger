#include "AssertionView.h"

#include "BasicTableView.h"
#include "SelectionDetailsWidget.h"
#include "SortFilterProxyModel.h"

#include <QAbstractItemModel>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpression>
#include <QSplitter>
#include <QVBoxLayout>

AssertionView::AssertionView(AppEventBus* xEventBus, QWidget* xParent)
    : QWidget(xParent)
    , dTableView(nullptr)
    , dFilterEdit(nullptr)
    , dProxyModel(nullptr)
    , dDetailsWidget(nullptr)
    , dSplitter(nullptr)
{
    mCreateGui(xEventBus);
}

void AssertionView::mCreateGui(AppEventBus* xEventBus)
{
    dTableView = new BasicTableView(xEventBus, this);
    dFilterEdit = new QLineEdit(this);
    dProxyModel = new SortFilterProxyModel(this);
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

    dProxyModel->setSourceModel(xModel);
    dTableView->setModel(dProxyModel);
    dDetailsWidget->mClear();

    if (dTableView->selectionModel() != nullptr) {
        dSelectionConnection = connect(
            dTableView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            [this](const QModelIndex& xCurrent, const QModelIndex&) { mUpdateDetails(xCurrent); });
    }

    if (dProxyModel != nullptr) {
        dDataChangedConnection = connect(
            dProxyModel,
            &QAbstractItemModel::dataChanged,
            this,
            [this](const QModelIndex&, const QModelIndex&, const QList<int>&) {
                mUpdateDetails(dTableView->currentIndex());
            });
    }
}

void AssertionView::mApplyFilter(const QString& xText)
{
    if (dProxyModel == nullptr) {
        return;
    }

    dProxyModel->setFilterRegularExpression(
        QRegularExpression(QRegularExpression::escape(xText), QRegularExpression::CaseInsensitiveOption));
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
