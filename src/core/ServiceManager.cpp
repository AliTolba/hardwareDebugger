#include "ServiceManager.h"

ServiceManager::ServiceManager(QObject* xParent)
    : QObject(xParent)
    , dEventBus(this)
    , dDatabase(this)
    , dDesignRepository(&dDatabase)
    , dSignalRepository(&dDatabase)
    , dAssertionRepository(&dDatabase)
    , dTransactionRepository(&dDatabase)
    , dDashboardService(&dDesignRepository, &dSignalRepository, &dAssertionRepository, &dTransactionRepository)
    , dDesignViewService(&dDesignRepository, &dEventBus)
    , dSignalViewService(&dSignalRepository, &dEventBus)
    , dAssertionViewService(&dAssertionRepository, &dEventBus)
    , dTransactionViewService(&dTransactionRepository, &dEventBus)
{
}

bool ServiceManager::mInitialize()
{
    if (!dDatabase.mInitialize()) {
        return false;
    }

    dEventBus.mPublishDatasetChanged();
    return true;
}

bool ServiceManager::mRebuildDemoData()
{
    const bool lOk = dDatabase.mReseed();
    if (lOk) {
        dEventBus.mPublishDatasetChanged();
    }

    return lOk;
}

DatabaseManager* ServiceManager::mDatabase()
{
    return &dDatabase;
}

AppEventBus* ServiceManager::mEventBus()
{
    return &dEventBus;
}

DashboardService* ServiceManager::mDashboardService()
{
    return &dDashboardService;
}

DesignViewService* ServiceManager::mDesignViewService()
{
    return &dDesignViewService;
}

SignalViewService* ServiceManager::mSignalViewService()
{
    return &dSignalViewService;
}

AssertionViewService* ServiceManager::mAssertionViewService()
{
    return &dAssertionViewService;
}

TransactionViewService* ServiceManager::mTransactionViewService()
{
    return &dTransactionViewService;
}
