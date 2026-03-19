#pragma once

#include <QPersistentModelIndex>
#include <QTreeView>
#include <functional>

class AppEventBus;
class QThread;

class BasicTreeView : public QTreeView
{
    Q_OBJECT

public:
    explicit BasicTreeView(AppEventBus* xEventBus, QWidget* xParent = nullptr);
    ~BasicTreeView() override;

protected:
    bool viewportEvent(QEvent* xEvent) override;

private slots:
    void mShowContextMenu(const QPoint& xPosition);
    void mRunExpandLoaded();
    void mRunCollapseLoaded();
    void mRunFetchCurrentBranch();
    void mOnNodeExpanded(const QModelIndex& xIndex);

private:
    void mExecuteIndexedAction(
        const QString& xTaskLabel,
        const QList<QPersistentModelIndex>& xIndexes,
        const std::function<void(const QModelIndex&)>& xAction);
    void mRunInBackground(const QString& xTaskLabel, int xSteps, const std::function<void(int)>& xAction);
    QList<QPersistentModelIndex> mCollectLoadedIndexes() const;
    void mCollectLoadedIndexes(const QModelIndex& xParent, QList<QPersistentModelIndex>& xIndexes) const;
    void mCleanupThread();

    AppEventBus* dEventBus;
    QThread* dWorkerThread;
};