#pragma once

class DesignRepository;
class SignalRepository;
class AssertionRepository;
class TransactionRepository;

struct DashboardStats
{
    int designNodes = 0;
    int signalNodes = 0;
    int assertions = 0;
    int assertionFailures = 0;
    int transactions = 0;
    int openTransactions = 0;
};

class DashboardService
{
public:
    DashboardService(
        const DesignRepository* xDesignRepository = nullptr,
        const SignalRepository* xSignalRepository = nullptr,
        const AssertionRepository* xAssertionRepository = nullptr,
        const TransactionRepository* xTransactionRepository = nullptr);

    DashboardStats mStats() const;

private:
    const DesignRepository* dDesignRepository;
    const SignalRepository* dSignalRepository;
    const AssertionRepository* dAssertionRepository;
    const TransactionRepository* dTransactionRepository;
};
