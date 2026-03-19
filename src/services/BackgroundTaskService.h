#pragma once

#include <QObject>
#include <QString>
#include <functional>

class AppEventBus;
class QThread;

class BackgroundTaskService : public QObject
{
    Q_OBJECT

public:
    explicit BackgroundTaskService(AppEventBus* xEventBus, QObject* xParent = nullptr);
    ~BackgroundTaskService() override;

    bool mRunTask(const QString& xTaskLabel, const std::function<void()>& xTaskBody);

private:
    void mCleanupThread();

    AppEventBus* dEventBus;
    QThread* dWorkerThread;
};
