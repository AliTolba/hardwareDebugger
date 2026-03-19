#include "BasicTableView.h"

#include "AppEventBus.h"
#include "BasicTableViewModel.h"
#include "DataLoadingWorker.h"

#include <QEvent>
#include <QHeaderView>
#include <QHelpEvent>
#include <QMenu>
#include <QThread>
#include <QToolTip>

BasicTableView::BasicTableView(AppEventBus* xEventBus, QWidget* xParent)
    : QTableView(xParent)
    , dEventBus(xEventBus)
    , dWorkerThread(nullptr)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setVisible(false);

    connect(this, &QWidget::customContextMenuRequested, this, &BasicTableView::mShowContextMenu);
}

BasicTableView::~BasicTableView()
{
    mCleanupThread();
}

bool BasicTableView::viewportEvent(QEvent* xEvent)
{
    if (xEvent->type() == QEvent::ToolTip)
    {
        auto* lHelpEvent = static_cast<QHelpEvent*>(xEvent);
        const QModelIndex lIndex = indexAt(lHelpEvent->pos());

        if (lIndex.isValid() && model() != nullptr)
        {
            QString lTooltip = model()->data(lIndex, Qt::ToolTipRole).toString();
            if (lTooltip.isEmpty())
            {
                lTooltip = model()->data(lIndex, Qt::DisplayRole).toString();
            }

            if (!lTooltip.isEmpty())
            {
                QToolTip::showText(lHelpEvent->globalPos(), lTooltip, viewport(), visualRect(lIndex));
                return true;
            }
        }

        QToolTip::hideText();
        xEvent->ignore();
        return true;
    }

    return QTableView::viewportEvent(xEvent);
}

void BasicTableView::mShowContextMenu(const QPoint& xPosition)
{
    QMenu lMenu(this);
    lMenu.addAction(QStringLiteral("Fetch next batch"), this, &BasicTableView::mRunFetchNextBatch);
    lMenu.addAction(QStringLiteral("Fetch all remaining rows"), this, &BasicTableView::mRunFetchAll);
    lMenu.addSeparator();
    lMenu.addAction(QStringLiteral("Reset lazy cache"), this, &BasicTableView::mRunResetView);
    lMenu.exec(viewport()->mapToGlobal(xPosition));
}

void BasicTableView::mRunFetchNextBatch()
{
    mRunInBackground(QStringLiteral("Fetching next row batch"), 1, [this](int) {
        auto* lTableModel = qobject_cast<BasicTableViewModel*>(model());
        if (lTableModel != nullptr && lTableModel->canFetchMore(QModelIndex())) {
            lTableModel->fetchMore(QModelIndex());
        }
    });
}

void BasicTableView::mRunFetchAll()
{
    auto* lTableModel = qobject_cast<BasicTableViewModel*>(model());
    if (lTableModel == nullptr) {
        return;
    }

    mRunInBackground(QStringLiteral("Fetching all remaining rows"), lTableModel->mRemainingBatches(), [this](int) {
        auto* lCurrentModel = qobject_cast<BasicTableViewModel*>(model());
        if (lCurrentModel != nullptr && lCurrentModel->canFetchMore(QModelIndex())) {
            lCurrentModel->fetchMore(QModelIndex());
        }
    });
}

void BasicTableView::mRunResetView()
{
    mRunInBackground(QStringLiteral("Resetting lazy table view"), 1, [this](int) {
        auto* lTableModel = qobject_cast<BasicTableViewModel*>(model());
        if (lTableModel != nullptr) {
            lTableModel->mResetCache();
        }
    });
}

void BasicTableView::mRunInBackground(const QString& xTaskLabel, int xSteps, const std::function<void(int)>& xAction)
{
    if (xSteps <= 0 || dWorkerThread != nullptr) {
        return;
    }

    DataLoadingWorker* lWorker = new DataLoadingWorker(xTaskLabel, xSteps, 10);
    dWorkerThread = new QThread(this);
    lWorker->moveToThread(dWorkerThread);

    connect(dWorkerThread, &QThread::started, lWorker, &DataLoadingWorker::mRun);
    connect(lWorker, &DataLoadingWorker::mStepReady, this, [xAction](int xStep) { xAction(xStep); });
    connect(lWorker, &DataLoadingWorker::mProgress, this, [this](const QString& xTaskName, int xProgress) {
        if (dEventBus != nullptr) {
            dEventBus->mPublishProgress(xTaskName, xProgress);
        }
    });
    connect(lWorker, &DataLoadingWorker::mFinished, this, [this, lWorker, xTaskLabel]() {
        if (dEventBus != nullptr) {
            dEventBus->mPublishProgress(QStringLiteral("Finished %1").arg(xTaskLabel), 100);
            dEventBus->mPublishBusyState("Ready", false);
        }
        lWorker->deleteLater();
        mCleanupThread();
    });

    if (dEventBus != nullptr) {
        dEventBus->mPublishBusyState(xTaskLabel, true);
        dEventBus->mPublishProgress(xTaskLabel, 0);
    }

    dWorkerThread->start();
}

void BasicTableView::mCleanupThread()
{
    if (dWorkerThread == nullptr) {
        return;
    }

    dWorkerThread->quit();
    dWorkerThread->wait();
    dWorkerThread->deleteLater();
    dWorkerThread = nullptr;
}