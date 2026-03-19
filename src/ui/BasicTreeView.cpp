#include "BasicTreeView.h"

#include "AppEventBus.h"
#include "DataLoadingWorker.h"

#include <algorithm>
#include <QEvent>
#include <QHelpEvent>
#include <QMenu>
#include <QThread>
#include <QToolTip>

BasicTreeView::BasicTreeView(AppEventBus* xEventBus, QWidget* xParent)
    : QTreeView(xParent)
    , dEventBus(xEventBus)
    , dWorkerThread(nullptr)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setAlternatingRowColors(true);
    setUniformRowHeights(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    connect(this, &QWidget::customContextMenuRequested, this, &BasicTreeView::mShowContextMenu);
    connect(this, &QTreeView::expanded, this, &BasicTreeView::mOnNodeExpanded);
}

BasicTreeView::~BasicTreeView()
{
    mCleanupThread();
}

bool BasicTreeView::viewportEvent(QEvent* xEvent)
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

    return QTreeView::viewportEvent(xEvent);
}

void BasicTreeView::mShowContextMenu(const QPoint& xPosition)
{
    QMenu lMenu(this);
    lMenu.addAction(QStringLiteral("Expand loaded hierarchy"), this, &BasicTreeView::mRunExpandLoaded);
    lMenu.addAction(QStringLiteral("Collapse loaded hierarchy"), this, &BasicTreeView::mRunCollapseLoaded);
    lMenu.addSeparator();
    lMenu.addAction(QStringLiteral("Load remaining children for selected node"), this, &BasicTreeView::mRunFetchCurrentBranch);
    lMenu.exec(viewport()->mapToGlobal(xPosition));
}

void BasicTreeView::mRunExpandLoaded()
{
    QList<QPersistentModelIndex> lIndexes = mCollectLoadedIndexes();
    mExecuteIndexedAction(QStringLiteral("Expanding loaded hierarchy"), lIndexes, [this](const QModelIndex& xIndex) {
        if (xIndex.isValid()) {
            expand(xIndex);
        }
    });
}

void BasicTreeView::mRunCollapseLoaded()
{
    QList<QPersistentModelIndex> lIndexes = mCollectLoadedIndexes();
    std::reverse(lIndexes.begin(), lIndexes.end());
    mExecuteIndexedAction(QStringLiteral("Collapsing loaded hierarchy"), lIndexes, [this](const QModelIndex& xIndex) {
        if (xIndex.isValid()) {
            collapse(xIndex);
        }
    });
}

void BasicTreeView::mRunFetchCurrentBranch()
{
    const QPersistentModelIndex lCurrent = currentIndex();
    if (!lCurrent.isValid() || model() == nullptr || !model()->canFetchMore(lCurrent)) {
        return;
    }

    mRunInBackground(QStringLiteral("Loading selected branch batch"), 1, [this, lCurrent](int) {
        if (model() != nullptr && model()->canFetchMore(lCurrent)) {
            model()->fetchMore(lCurrent);
            expand(lCurrent);
        }
    });
}

void BasicTreeView::mOnNodeExpanded(const QModelIndex& xIndex)
{
    if (model() != nullptr && model()->canFetchMore(xIndex)) {
        model()->fetchMore(xIndex);
    }
}

void BasicTreeView::mExecuteIndexedAction(
    const QString& xTaskLabel,
    const QList<QPersistentModelIndex>& xIndexes,
    const std::function<void(const QModelIndex&)>& xAction)
{
    mRunInBackground(xTaskLabel, xIndexes.size(), [xIndexes, xAction](int xStep) {
        if (xStep >= 0 && xStep < xIndexes.size()) {
            xAction(xIndexes.at(xStep));
        }
    });
}

void BasicTreeView::mRunInBackground(const QString& xTaskLabel, int xSteps, const std::function<void(int)>& xAction)
{
    if (dWorkerThread != nullptr) {
        return;
    }

    DataLoadingWorker* lWorker = new DataLoadingWorker(xTaskLabel, xSteps, 0);
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
        dEventBus->mPublishProgress(QStringLiteral("Started %1").arg(xTaskLabel), 0);
    }

    dWorkerThread->start();
}

QList<QPersistentModelIndex> BasicTreeView::mCollectLoadedIndexes() const
{
    QList<QPersistentModelIndex> lIndexes;
    mCollectLoadedIndexes(QModelIndex(), lIndexes);
    return lIndexes;
}

void BasicTreeView::mCollectLoadedIndexes(const QModelIndex& xParent, QList<QPersistentModelIndex>& xIndexes) const
{
    if (model() == nullptr) {
        return;
    }

    const int lRows = model()->rowCount(xParent);
    for (int lRow = 0; lRow < lRows; ++lRow) {
        const QModelIndex lChild = model()->index(lRow, 0, xParent);
        xIndexes.append(QPersistentModelIndex(lChild));
        mCollectLoadedIndexes(lChild, xIndexes);
    }
}

void BasicTreeView::mCleanupThread()
{
    if (dWorkerThread == nullptr) {
        return;
    }

    dWorkerThread->quit();
    dWorkerThread->wait();
    dWorkerThread->deleteLater();
    dWorkerThread = nullptr;
}