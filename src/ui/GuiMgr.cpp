#include "GuiMgr.h"

#include "AssertionView.h"
#include "BasicTableViewModel.h"
#include "BasicTreeViewModel.h"
#include "DesignView.h"
#include "ServiceManager.h"
#include "SignalView.h"
#include "TransactionTableViewModel.h"
#include "TransactionView.h"

#include <QHeaderView>
#include <QStackedWidget>

GuiMgr::GuiMgr(ServiceManager* xServices, QObject* xParent)
    : QObject(xParent)
    , dServices(xServices)
    , dDesignView(nullptr)
    , dSignalView(nullptr)
    , dAssertionView(nullptr)
    , dTransactionView(nullptr)
    , dDesignModel(nullptr)
    , dSignalModel(nullptr)
    , dAssertionModel(nullptr)
    , dTransactionModel(nullptr)
{
}

void GuiMgr::mCreateViews(QStackedWidget* xViewStack)
{
    if (xViewStack == nullptr || dServices == nullptr) {
        return;
    }

    dDesignView = new DesignView(dServices->mEventBus(), xViewStack);
    dSignalView = new SignalView(dServices->mEventBus(), xViewStack);
    dAssertionView = new AssertionView(dServices->mEventBus(), xViewStack);
    dTransactionView = new TransactionView(dServices->mEventBus(), xViewStack);

    xViewStack->addWidget(dDesignView);
    xViewStack->addWidget(dSignalView);
    xViewStack->addWidget(dAssertionView);
    xViewStack->addWidget(dTransactionView);
}

void GuiMgr::mResetModels(QWidget* xModelParent)
{
    if (dServices == nullptr
        || dDesignView == nullptr
        || dSignalView == nullptr
        || dAssertionView == nullptr
        || dTransactionView == nullptr) {
        return;
    }

    delete dDesignModel;
    delete dSignalModel;
    delete dAssertionModel;
    delete dTransactionModel;

    dDesignModel = new BasicTreeViewModel(dServices->mDesignViewService(), 32, xModelParent);
    dSignalModel = new BasicTreeViewModel(dServices->mSignalViewService(), 32, xModelParent);
    dAssertionModel = new BasicTableViewModel(dServices->mAssertionViewService(), 400, xModelParent);
    dTransactionModel = new TransactionTableViewModel(dServices->mTransactionViewService(), 400, xModelParent);

    dDesignView->setModel(dDesignModel);
    dSignalView->setModel(dSignalModel);
    dAssertionView->setModel(dAssertionModel);
    dTransactionView->setModel(dTransactionModel);

    dDesignView->header()->setStretchLastSection(true);
    dSignalView->header()->setStretchLastSection(true);
    dAssertionView->resizeColumnsToContents();
    dTransactionView->resizeColumnsToContents();
}

QAbstractItemModel* GuiMgr::mModelForViewIndex(int xIndex) const
{
    switch (xIndex) {
        case 0: return dDesignModel;
        case 1: return dSignalModel;
        case 2: return dAssertionModel;
        case 3: return dTransactionModel;
        default: return nullptr;
    }
}

TransactionView* GuiMgr::mTransactionView() const
{
    return dTransactionView;
}
