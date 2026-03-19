#pragma once

#include <QPersistentModelIndex>
#include <QTreeView>

class AppEventBus;

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
    QList<QPersistentModelIndex> mCollectLoadedIndexes() const;
    void mCollectLoadedIndexes(const QModelIndex& xParent, QList<QPersistentModelIndex>& xIndexes) const;
};