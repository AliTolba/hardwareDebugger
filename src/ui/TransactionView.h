#pragma once

#include <QMetaObject>
#include <QWidget>

class AppEventBus;
class BasicTableView;
class QAbstractItemModel;
class QLineEdit;
class QModelIndex;
class QSplitter;
class SelectionDetailsWidget;
class SortFilterProxyModel;

class TransactionView : public QWidget
{
    Q_OBJECT

public:
    explicit TransactionView(AppEventBus* xEventBus, QWidget* xParent = nullptr);
    void setModel(QAbstractItemModel* xModel);
    void resizeColumnsToContents();

signals:
    void mTransactionEdited();

private:
    void mCreateGui(AppEventBus* xEventBus);
    void mApplyFilter(const QString& xText);
    void mUpdateDetails(const QModelIndex& xCurrent);

    BasicTableView* dTableView;
    QLineEdit* dFilterEdit;
    SortFilterProxyModel* dProxyModel;
    SelectionDetailsWidget* dDetailsWidget;
    QSplitter* dSplitter;
    QMetaObject::Connection dModelConnection;
    QMetaObject::Connection dSelectionConnection;
    QMetaObject::Connection dDataChangedConnection;
};
