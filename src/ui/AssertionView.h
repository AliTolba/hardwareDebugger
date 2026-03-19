#pragma once

#include <QMetaObject>
#include <QWidget>

class AppEventBus;
class BasicTableView;
class QLineEdit;
class QAbstractItemModel;
class QModelIndex;
class QSplitter;
class SelectionDetailsWidget;
class SortFilterProxyModel;

class AssertionView : public QWidget
{
    Q_OBJECT

public:
    explicit AssertionView(AppEventBus* xEventBus, QWidget* xParent = nullptr);

    void setModel(QAbstractItemModel* xModel);
    void resizeColumnsToContents();

private:
    void mCreateGui(AppEventBus* xEventBus);
    void mApplyFilter(const QString& xText);
    void mUpdateDetails(const QModelIndex& xCurrent);

    BasicTableView* dTableView;
    QLineEdit* dFilterEdit;
    SortFilterProxyModel* dProxyModel;
    SelectionDetailsWidget* dDetailsWidget;
    QSplitter* dSplitter;
    QMetaObject::Connection dSelectionConnection;
    QMetaObject::Connection dDataChangedConnection;
};
