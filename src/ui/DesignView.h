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
class BasicTreeViewModel;

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
    void mHandleColumnSorted(int xLogicalIndex, Qt::SortOrder xOrder);

    BasicTreeView* dTreeView;
    QLineEdit* dFilterEdit;
    SelectionDetailsWidget* dDetailsWidget;
    QSplitter* dSplitter;
    BasicTreeViewModel* dTreeModel;
    QMetaObject::Connection dSelectionConnection;
    QMetaObject::Connection dDataChangedConnection;
};
