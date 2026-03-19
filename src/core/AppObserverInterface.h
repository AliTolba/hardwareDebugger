#pragma once

#include <QString>

class AppObserverInterface
{
public:
    virtual ~AppObserverInterface() = default;

    virtual void mOnProgressChanged(const QString& xTaskName, int xProgress) = 0;
    virtual void mOnBusyStateChanged(const QString& xTaskName, bool xBusy) = 0;
    virtual void mOnDatasetChanged() = 0;
};
