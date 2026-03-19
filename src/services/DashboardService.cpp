#include "DashboardService.h"

#include "AssertionRepository.h"
#include "DesignRepository.h"
#include "SignalRepository.h"
#include "TransactionRepository.h"

DashboardService::DashboardService(
    const DesignRepository* xDesignRepository,
    const SignalRepository* xSignalRepository,
    const AssertionRepository* xAssertionRepository,
    const TransactionRepository* xTransactionRepository)
    : dDesignRepository(xDesignRepository)
    , dSignalRepository(xSignalRepository)
    , dAssertionRepository(xAssertionRepository)
    , dTransactionRepository(xTransactionRepository)
{
}

DashboardStats DashboardService::mStats() const
{
    DashboardStats lSnapshot;
    lSnapshot.designNodes = dDesignRepository != nullptr ? dDesignRepository->mTotalCount() : 0;
    lSnapshot.signalNodes = dSignalRepository != nullptr ? dSignalRepository->mTotalCount() : 0;
    lSnapshot.assertions = dAssertionRepository != nullptr ? dAssertionRepository->mTotalCount() : 0;
    lSnapshot.assertionFailures = dAssertionRepository != nullptr ? dAssertionRepository->mFailureCount() : 0;
    lSnapshot.transactions = dTransactionRepository != nullptr ? dTransactionRepository->mTotalCount() : 0;
    lSnapshot.openTransactions = dTransactionRepository != nullptr ? dTransactionRepository->mOpenCount() : 0;
    return lSnapshot;
}
