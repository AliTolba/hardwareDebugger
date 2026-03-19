#include "DataExporter.h"

#include "AppEventBus.h"
#include "BasicTableViewModel.h"
#include "ExportStrategyInterface.h"
#include "DataLoadingWorker.h"

#include <QAbstractItemModel>
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QModelIndex>
#include <QPointer>
#include <QThread>

namespace {

struct ExportResult
{
    bool dOk = false;
    QString dErrorMessage;
    QString dSavedFilePath;
};

void sPublishBusyState(AppEventBus* xEventBus, const QString& xTaskName, bool xBusy)
{
    if (xEventBus != nullptr) {
        xEventBus->mPublishBusyState(xTaskName, xBusy);
    }
}

void sPublishProgress(AppEventBus* xEventBus, const QString& xTaskName, int xProgress)
{
    if (xEventBus != nullptr) {
        xEventBus->mPublishProgress(xTaskName, xProgress);
    }
}

void sEnsureModelFullyLoaded(QAbstractItemModel* xModel, const QModelIndex& xParent)
{
    if (xModel == nullptr) {
        return;
    }

    while (xModel->canFetchMore(xParent)) {
        xModel->fetchMore(xParent);
    }

    const int lRowCount = xModel->rowCount(xParent);
    for (int lRow = 0; lRow < lRowCount; ++lRow) {
        const QModelIndex lChild = xModel->index(lRow, 0, xParent);
        sEnsureModelFullyLoaded(xModel, lChild);
    }
}

QStringList sCollectHeaders(QAbstractItemModel* xModel)
{
    QStringList lHeaders;
    if (xModel == nullptr) {
        return lHeaders;
    }

    const int lColumnCount = xModel->columnCount();
    lHeaders.reserve(lColumnCount);
    for (int lCol = 0; lCol < lColumnCount; ++lCol) {
        lHeaders.append(xModel->headerData(lCol, Qt::Horizontal).toString());
    }

    return lHeaders;
}

void sCollectRows(QAbstractItemModel* xModel,
                  const QModelIndex& xParent,
                  QVector<QVector<QString>>& xRows,
                  int xDepth)
{
    if (xModel == nullptr) {
        return;
    }

    const int lRowCount = xModel->rowCount(xParent);
    const int lColCount = xModel->columnCount(xParent);

    for (int lRow = 0; lRow < lRowCount; ++lRow) {
        QVector<QString> lRowData;
        lRowData.reserve(lColCount);
        for (int lCol = 0; lCol < lColCount; ++lCol) {
            QString lVal = xModel->data(xModel->index(lRow, lCol, xParent)).toString();
            if (lCol == 0 && xDepth > 0) {
                lVal.prepend(QString(xDepth * 2, QLatin1Char(' ')));
            }
            lRowData.append(lVal);
        }
        xRows.append(lRowData);

        const QModelIndex lIndex = xModel->index(lRow, 0, xParent);
        if (xModel->hasChildren(lIndex)) {
            sCollectRows(xModel, lIndex, xRows, xDepth + 1);
        }
    }
}

void sWriteInWorkerThread(const std::shared_ptr<ExportStrategyInterface>& xStrategy,
                          const QStringList& xHeaders,
                          const QVector<QVector<QString>>& xRows,
                          const QString& xFilePath,
                          QWidget* xParent,
                          AppEventBus* xEventBus,
                          const std::function<void(const QString&)>& xOnSuccess,
                          const std::function<void(const QString&)>& xOnError)
{
    if (xStrategy == nullptr) {
        if (xOnError) {
            xOnError(QStringLiteral("Export strategy is not available."));
        }
        return;
    }

    const QString lTaskName = QStringLiteral("Writing export file");
    sPublishBusyState(xEventBus, lTaskName, true);
    sPublishProgress(xEventBus, lTaskName, 0);

    auto lResult = std::make_shared<ExportResult>();
    QPointer<QWidget> lParentGuard(xParent);

    QThread* lThread = QThread::create([xStrategy, lResult, xHeaders, xRows, xFilePath]() {
        QFile lFile(xFilePath);
        if (!lFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            lResult->dOk = false;
            lResult->dErrorMessage = QStringLiteral("Cannot open file for writing:\n%1").arg(xFilePath);
            return;
        }

        lResult->dOk = xStrategy->mExport(xHeaders, xRows, &lFile);
        lResult->dSavedFilePath = xFilePath;
        lFile.close();

        if (!lResult->dOk && lResult->dErrorMessage.isEmpty()) {
            lResult->dErrorMessage = QStringLiteral("An error occurred while writing the file.");
        }
    });

    QObject* lContext = xParent != nullptr ? static_cast<QObject*>(xParent) : static_cast<QObject*>(qApp);
    QObject::connect(lThread, &QThread::finished, lContext, [lResult, lParentGuard, xOnSuccess, xOnError, xEventBus, lTaskName]() {
        sPublishProgress(xEventBus, lTaskName, 100);
        sPublishBusyState(xEventBus, lTaskName, false);

        if (lResult->dOk) {
            if (xOnSuccess) {
                xOnSuccess(lResult->dSavedFilePath);
            }
            return;
        }

        if (xOnError) {
            xOnError(lResult->dErrorMessage);
            return;
        }

        if (lParentGuard != nullptr) {
            QMessageBox::warning(lParentGuard,
                                 QStringLiteral("Export failed"),
                                 lResult->dErrorMessage);
        }
    });

    QObject::connect(lThread, &QThread::finished, lThread, &QObject::deleteLater);
    lThread->start();
}

void sFinalizeExport(QAbstractItemModel* xModel,
                     const std::shared_ptr<ExportStrategyInterface>& xStrategy,
                     const QString& xFilePath,
                     QWidget* xParent,
                     AppEventBus* xEventBus,
                     const std::function<void(const QString&)>& xOnSuccess,
                     const std::function<void(const QString&)>& xOnError)
{
    const QString lPrepareTaskName = QStringLiteral("Preparing export data");
    sPublishProgress(xEventBus, QStringLiteral("Collecting export rows"), 0);

    const QStringList lHeaders = sCollectHeaders(xModel);
    QVector<QVector<QString>> lRows;
    sCollectRows(xModel, QModelIndex(), lRows, 0);

    sPublishProgress(xEventBus, lPrepareTaskName, 100);
    sPublishBusyState(xEventBus, lPrepareTaskName, false);

    sWriteInWorkerThread(xStrategy, lHeaders, lRows, xFilePath, xParent, xEventBus, xOnSuccess, xOnError);
}

bool sStartTableLoadPreparation(QAbstractItemModel* xModel,
                                const std::shared_ptr<ExportStrategyInterface>& xStrategy,
                                const QString& xFilePath,
                                QWidget* xParent,
                                AppEventBus* xEventBus,
                                const std::function<void(const QString&)>& xOnSuccess,
                                const std::function<void(const QString&)>& xOnError)
{
    auto* lTableModel = qobject_cast<BasicTableViewModel*>(xModel);
    if (lTableModel == nullptr) {
        return false;
    }

    const int lRemainingBatches = lTableModel->mRemainingBatches();
    if (lRemainingBatches <= 0) {
        return false;
    }

    const QString lTaskName = QStringLiteral("Preparing export data");
    sPublishBusyState(xEventBus, lTaskName, true);
    sPublishProgress(xEventBus, lTaskName, 0);

    QObject* lContext = xParent != nullptr ? static_cast<QObject*>(xParent) : static_cast<QObject*>(qApp);
    QPointer<QAbstractItemModel> lModelGuard(xModel);

    auto* lWorker = new DataLoadingWorker(lTaskName, lRemainingBatches, 0);
    auto* lThread = new QThread(lContext);
    lWorker->moveToThread(lThread);

    QObject::connect(lThread, &QThread::started, lWorker, &DataLoadingWorker::mRun);
    QObject::connect(lWorker, &DataLoadingWorker::mStepReady, lContext, [lModelGuard](int) {
        auto* lCurrentModel = qobject_cast<BasicTableViewModel*>(lModelGuard.data());
        if (lCurrentModel != nullptr && lCurrentModel->canFetchMore(QModelIndex())) {
            lCurrentModel->fetchMore(QModelIndex());
        }
    });
    QObject::connect(lWorker, &DataLoadingWorker::mProgress, lContext, [xEventBus](const QString& xTaskName, int xProgress) {
        sPublishProgress(xEventBus, xTaskName, xProgress);
    });
    QObject::connect(lWorker, &DataLoadingWorker::mFinished, lContext, [lThread, lWorker, lModelGuard, xStrategy, xFilePath, xParent, xEventBus, xOnSuccess, xOnError]() {
        lWorker->deleteLater();
        lThread->quit();

        if (lModelGuard == nullptr) {
            sPublishBusyState(xEventBus, QStringLiteral("Preparing export data"), false);
            if (xOnError) {
                xOnError(QStringLiteral("Export model is no longer available."));
            }
            return;
        }

        sFinalizeExport(lModelGuard.data(), xStrategy, xFilePath, xParent, xEventBus, xOnSuccess, xOnError);
    });
    QObject::connect(lThread, &QThread::finished, lThread, &QObject::deleteLater);

    lThread->start();
    return true;
}

} // namespace

DataExporter::DataExporter(ExportStrategyInterface* xStrategy, AppEventBus* xEventBus)
    : dStrategy(std::shared_ptr<ExportStrategyInterface>(xStrategy))
    , dEventBus(xEventBus)
{
}

bool DataExporter::mExport(QAbstractItemModel* xModel,
                           QWidget* xParent,
                           const std::function<void(const QString&)>& xOnSuccess,
                           const std::function<void(const QString&)>& xOnError) const
{
    if (dStrategy == nullptr || xModel == nullptr) {
        return false;
    }

    // --- build file path via dialog ---
    const QString lFilter = QStringLiteral("%1 files (*.%2);;All files (*)")
                                .arg(dStrategy->mFormatName())
                                .arg(dStrategy->mFileExtension());

    const QString lFilePath = QFileDialog::getSaveFileName(
        xParent,
        QStringLiteral("Export Current View as %1").arg(dStrategy->mFormatName()),
        QString(),
        lFilter);

    if (lFilePath.isEmpty()) {
        return false; // user cancelled before start
    }

    const std::shared_ptr<ExportStrategyInterface> lStrategy = dStrategy;
    if (sStartTableLoadPreparation(xModel, lStrategy, lFilePath, xParent, dEventBus, xOnSuccess, xOnError)) {
        return true;
    }

    // Qt item models are generally thread-affine to the UI thread, so tree/generic
    // model fetchMore/data traversal must remain on the owning thread.
    const QString lPrepareTaskName = QStringLiteral("Preparing export data");
    sPublishBusyState(dEventBus, lPrepareTaskName, true);
    sPublishProgress(dEventBus, lPrepareTaskName, 0);

    sEnsureModelFullyLoaded(xModel, QModelIndex());
    sFinalizeExport(xModel, lStrategy, lFilePath, xParent, dEventBus, xOnSuccess, xOnError);
    return true;
}
