#pragma once

#include <QObject>

#include "AppEventBus.h"
#include "DatabaseManager.h"
#include "AssertionRepository.h"
#include "DesignRepository.h"
#include "SignalRepository.h"
#include "TransactionRepository.h"
#include "AssertionViewService.h"
#include "DashboardService.h"
#include "DesignViewService.h"
#include "SignalViewService.h"
#include "TransactionViewService.h"

class ServiceManager : public QObject
{
    Q_OBJECT

public:
    explicit ServiceManager(QObject* xParent = nullptr);

    bool mInitialize();
    bool mRebuildDemoData();

    DatabaseManager* mDatabase();
    AppEventBus* mEventBus();
    DashboardService* mDashboardService();
    DesignViewService* mDesignViewService();
    SignalViewService* mSignalViewService();
    AssertionViewService* mAssertionViewService();
    TransactionViewService* mTransactionViewService();

private:
    AppEventBus dEventBus;
    DatabaseManager dDatabase;
    DesignRepository dDesignRepository;
    SignalRepository dSignalRepository;
    AssertionRepository dAssertionRepository;
    TransactionRepository dTransactionRepository;
    DashboardService dDashboardService;
    DesignViewService dDesignViewService;
    SignalViewService dSignalViewService;
    AssertionViewService dAssertionViewService;
    TransactionViewService dTransactionViewService;
};
