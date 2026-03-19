#pragma once

#include <QObject>
#include <QList>
#include <QString>

class AppObserverInterface;

class AppEventBus : public QObject
{
    Q_OBJECT

public:
    explicit AppEventBus(QObject* xParent = nullptr);

    void mAttach(AppObserverInterface* xObserver);
    void mDetach(AppObserverInterface* xObserver);

    void mPublishProgress(const QString& xTaskName, int xProgress);
    void mPublishBusyState(const QString& xTaskName, bool xBusy);
    void mPublishDatasetChanged();

signals:
    void mProgressChanged(const QString& xTaskName, int xProgress);
    void mBusyStateChanged(const QString& xTaskName, bool xBusy);
    void mDatasetChanged();

private:
    QList<AppObserverInterface*> dObservers;
};
