#include "DataLoadingWorker.h"

#include <QThread>

DataLoadingWorker::DataLoadingWorker(const QString& xTaskName, int xSteps, int xDelayMs, QObject* xParent)
    : QObject(xParent)
    , dTaskName(xTaskName)
    , dSteps(xSteps)
    , dDelayMs(xDelayMs)
{
}

void DataLoadingWorker::mRun()
{
    if (dSteps <= 0) {
        emit mProgress(dTaskName, 100);
        emit mFinished();
        return;
    }

    for (int lI = 0; lI < dSteps; ++lI) {
        emit mStepReady(lI);
        emit mProgress(dTaskName, ((lI + 1) * 100) / dSteps);
        QThread::msleep(dDelayMs);
    }

    emit mFinished();
}
