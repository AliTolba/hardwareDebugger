#include "DesignView.h"

#include "BasicTreeView.h"
#include "SelectionDetailsWidget.h"
#include "SortFilterProxyModel.h"

#include <QAbstractItemModel>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QList>
#include <QLineEdit>
#include <QRegularExpression>
#include <QSplitter>
#include <QVBoxLayout>

DesignView::DesignView(AppEventBus* xEventBus, QWidget* xParent)
    : QWidget(xParent)
    , dTreeView(nullptr)
    , dFilterEdit(nullptr)
    , dProxyModel(nullptr)
    , dDetailsWidget(nullptr)
    , dSplitter(nullptr)
{
    mCreateGui(xEventBus);
}

void DesignView::mCreateGui(AppEventBus* xEventBus)
{
    dTreeView = new BasicTreeView(xEventBus, this);
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

    dTreeView->setSortingEnabled(false);

    connect(dFilterEdit, &QLineEdit::textChanged, this, &DesignView::mApplyFilter);

    dSplitter->addWidget(dTreeView);
    dSplitter->addWidget(dDetailsWidget);
    dSplitter->setStretchFactor(0, 4);
    dSplitter->setStretchFactor(1, 2);

    lLayout->addLayout(lFilterLayout);
    lLayout->addWidget(dSplitter);
}

void DesignView::setModel(QAbstractItemModel* xModel)
{
    if (dSelectionConnection) {
        disconnect(dSelectionConnection);
    }
    if (dDataChangedConnection) {
        disconnect(dDataChangedConnection);
    }

    dProxyModel->setSourceModel(xModel);
    dTreeView->setModel(dProxyModel);
    dDetailsWidget->mClear();

    if (dTreeView->selectionModel() != nullptr) {
        dSelectionConnection = connect(
            dTreeView->selectionModel(),
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
                mUpdateDetails(dTreeView->currentIndex());
            });
    }
}

void DesignView::mApplyFilter(const QString& xText)
{
    if (dProxyModel == nullptr) {
        return;
    }

    dProxyModel->setFilterRegularExpression(
        QRegularExpression(QRegularExpression::escape(xText), QRegularExpression::CaseInsensitiveOption));
    mUpdateDetails(dTreeView->currentIndex());
}

QHeaderView* DesignView::header() const
{
    return dTreeView->header();
}

void DesignView::mUpdateDetails(const QModelIndex& xCurrent)
{
    dDetailsWidget->mDisplayIndex(xCurrent);
}
