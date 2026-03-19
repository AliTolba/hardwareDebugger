#pragma once

#include <QMetaObject>
#include <QWidget>

class AppEventBus;
class BasicTreeView;
class QAbstractItemModel;
class QHeaderView;
class QLineEdit;
class QModelIndex;
class QSplitter;
class QString;
class SelectionDetailsWidget;
class SortFilterProxyModel;

class DesignView : public QWidget
{
    Q_OBJECT

public:
    explicit DesignView(AppEventBus* xEventBus, QWidget* xParent = nullptr);

    void setModel(QAbstractItemModel* xModel);
    QHeaderView* header() const;

private:
    void mCreateGui(AppEventBus* xEventBus);
    void mApplyFilter(const QString& xText);
    void mUpdateDetails(const QModelIndex& xCurrent);

    BasicTreeView* dTreeView;
    QLineEdit* dFilterEdit;
    SortFilterProxyModel* dProxyModel;
    SelectionDetailsWidget* dDetailsWidget;
    QSplitter* dSplitter;
    QMetaObject::Connection dSelectionConnection;
    QMetaObject::Connection dDataChangedConnection;
};
