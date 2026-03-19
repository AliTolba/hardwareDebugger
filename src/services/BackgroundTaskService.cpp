#include "BackgroundTaskService.h"

#include "AppEventBus.h"

#include <QThread>

BackgroundTaskService::BackgroundTaskService(AppEventBus* xEventBus, QObject* xParent)
    : QObject(xParent)
    , dEventBus(xEventBus)
    , dWorkerThread(nullptr)
{
}

BackgroundTaskService::~BackgroundTaskService()
{
    mCleanupThread();
}

bool BackgroundTaskService::mRunTask(const QString& xTaskLabel, const std::function<void()>& xTaskBody)
{
    if (dWorkerThread != nullptr || !xTaskBody) {
        return false;
    }

    dWorkerThread = QThread::create([xTaskBody]() {
        xTaskBody();
    });
    dWorkerThread->setParent(this);

    connect(dWorkerThread, &QThread::finished, this, [this, xTaskLabel]() {
        if (dEventBus != nullptr) {
            dEventBus->mPublishProgress(QStringLiteral("Finished %1").arg(xTaskLabel), -1);
            dEventBus->mPublishBusyState(QStringLiteral("Ready"), false);
        }
        mCleanupThread();
    });

    if (dEventBus != nullptr) {
        dEventBus->mPublishBusyState(xTaskLabel, true);
        dEventBus->mPublishProgress(xTaskLabel, -1);
    }

    dWorkerThread->start();
    return true;
}

void BackgroundTaskService::mCleanupThread()
{
    if (dWorkerThread == nullptr) {
        return;
    }

    if (dWorkerThread->isRunning()) {
        dWorkerThread->quit();
        dWorkerThread->wait();
    }

    dWorkerThread->deleteLater();
    dWorkerThread = nullptr;
}
