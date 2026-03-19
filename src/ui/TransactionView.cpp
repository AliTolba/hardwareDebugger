#include "TransactionView.h"

#include "BasicTableView.h"
#include "SelectionDetailsWidget.h"
#include "SortFilterProxyModel.h"
#include "TransactionTableViewModel.h"

#include <QAbstractItemModel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpression>
#include <QSplitter>
#include <QStyledItemDelegate>
#include <QVBoxLayout>

namespace {

class TransactionStatusDelegate : public QStyledItemDelegate
{
public:
    explicit TransactionStatusDelegate(QObject* xParent = nullptr)
        : QStyledItemDelegate(xParent)
    {
    }

    QWidget* createEditor(QWidget* xParent, const QStyleOptionViewItem&, const QModelIndex&) const override
    {
        auto* lCombo = new QComboBox(xParent);
        lCombo->addItems({
            QStringLiteral("Complete"),
            QStringLiteral("Pending"),
            QStringLiteral("Retry")
        });
        return lCombo;
    }

    void setEditorData(QWidget* xEditor, const QModelIndex& xIndex) const override
    {
        auto* lCombo = qobject_cast<QComboBox*>(xEditor);
        if (lCombo == nullptr) {
            return;
        }

        const QString lValue = xIndex.model()->data(xIndex, Qt::EditRole).toString();
        const int lFoundIndex = lCombo->findText(lValue);
        lCombo->setCurrentIndex(lFoundIndex >= 0 ? lFoundIndex : 0);
    }

    void setModelData(QWidget* xEditor, QAbstractItemModel* xModel, const QModelIndex& xIndex) const override
    {
        auto* lCombo = qobject_cast<QComboBox*>(xEditor);
        if (lCombo == nullptr || xModel == nullptr) {
            return;
        }

        xModel->setData(xIndex, lCombo->currentText(), Qt::EditRole);
    }
};

} // namespace

TransactionView::TransactionView(AppEventBus* xEventBus, QWidget* xParent)
    : QWidget(xParent)
    , dTableView(nullptr)
    , dFilterEdit(nullptr)
    , dProxyModel(nullptr)
    , dDetailsWidget(nullptr)
    , dSplitter(nullptr)
{
    mCreateGui(xEventBus);
}

void TransactionView::mCreateGui(AppEventBus* xEventBus)
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

    connect(dFilterEdit, &QLineEdit::textChanged, this, &TransactionView::mApplyFilter);

    dSplitter->addWidget(dTableView);
    dSplitter->addWidget(dDetailsWidget);
    dSplitter->setStretchFactor(0, 4);
    dSplitter->setStretchFactor(1, 2);

    lLayout->addLayout(lFilterLayout);
    lLayout->addWidget(dSplitter);

    dTableView->setItemDelegateForColumn(6, new TransactionStatusDelegate(dTableView));
}

void TransactionView::setModel(QAbstractItemModel* xModel)
{
    if (dSelectionConnection) {
        disconnect(dSelectionConnection);
    }
    if (dDataChangedConnection) {
        disconnect(dDataChangedConnection);
    }
    if (dModelConnection) {
        disconnect(dModelConnection);
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

    auto* lTransactionModel = qobject_cast<TransactionTableViewModel*>(xModel);
    if (lTransactionModel != nullptr) {
        dModelConnection = connect(
            lTransactionModel,
            &TransactionTableViewModel::mTransactionUpdated,
            this,
            &TransactionView::mTransactionEdited);
    }
}

void TransactionView::mApplyFilter(const QString& xText)
{
    if (dProxyModel == nullptr) {
        return;
    }

    dProxyModel->setFilterRegularExpression(
        QRegularExpression(QRegularExpression::escape(xText), QRegularExpression::CaseInsensitiveOption));
    mUpdateDetails(dTableView->currentIndex());
}

void TransactionView::resizeColumnsToContents()
{
    dTableView->resizeColumnsToContents();
}

void TransactionView::mUpdateDetails(const QModelIndex& xCurrent)
{
    dDetailsWidget->mDisplayIndex(xCurrent);
}
