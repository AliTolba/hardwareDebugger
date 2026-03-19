#pragma once

#include <QObject>
#include <QString>

class DataLoadingWorker : public QObject
{
    Q_OBJECT

public:
    explicit DataLoadingWorker(const QString& xTaskName, int xSteps, int xDelayMs = 8, QObject* xParent = nullptr);

public slots:
    void mRun();

signals:
    void mProgress(const QString& xTaskName, int xProgress);
    void mStepReady(int xStepIndex);
    void mFinished();

private:
    QString dTaskName;
    int dSteps;
    int dDelayMs;
};
