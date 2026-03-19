#pragma once

#include <QObject>

class AppEventBus;
class ServiceManager;
class QStackedWidget;
class QHeaderView;
class QAbstractItemModel;
class QWidget;
class DesignView;
class SignalView;
class AssertionView;
class TransactionView;
class BasicTreeViewModel;
class BasicTableViewModel;
class TransactionTableViewModel;

class GuiMgr : public QObject
{
    Q_OBJECT

public:
    explicit GuiMgr(ServiceManager* xServices, QObject* xParent = nullptr);

    void mCreateViews(QStackedWidget* xViewStack);
    void mResetModels(QWidget* xModelParent);

    QAbstractItemModel* mModelForViewIndex(int xIndex) const;
    TransactionView* mTransactionView() const;

private:
    ServiceManager* dServices;
    DesignView* dDesignView;
    SignalView* dSignalView;
    AssertionView* dAssertionView;
    TransactionView* dTransactionView;
    BasicTreeViewModel* dDesignModel;
    BasicTreeViewModel* dSignalModel;
    BasicTableViewModel* dAssertionModel;
    TransactionTableViewModel* dTransactionModel;
};
