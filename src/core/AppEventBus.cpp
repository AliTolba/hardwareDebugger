#include "AppEventBus.h"

#include "AppObserverInterface.h"

AppEventBus::AppEventBus(QObject* xParent)
    : QObject(xParent)
{
}

void AppEventBus::mAttach(AppObserverInterface* xObserver)
{
    if (xObserver != nullptr && !dObservers.contains(xObserver)) {
        dObservers.append(xObserver);
    }
}

void AppEventBus::mDetach(AppObserverInterface* xObserver)
{
    dObservers.removeAll(xObserver);
}

void AppEventBus::mPublishProgress(const QString& xTaskName, int xProgress)
{
    emit mProgressChanged(xTaskName, xProgress);
    for (AppObserverInterface* lObserver : dObservers) {
        lObserver->mOnProgressChanged(xTaskName, xProgress);
    }
}

void AppEventBus::mPublishBusyState(const QString& xTaskName, bool xBusy)
{
    emit mBusyStateChanged(xTaskName, xBusy);
    for (AppObserverInterface* lObserver : dObservers) {
        lObserver->mOnBusyStateChanged(xTaskName, xBusy);
    }
}

void AppEventBus::mPublishDatasetChanged()
{
    emit mDatasetChanged();
    for (AppObserverInterface* lObserver : dObservers) {
        lObserver->mOnDatasetChanged();
    }
}
