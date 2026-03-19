#include "BasicTreeView.h"

#include <algorithm>
#include <QEvent>
#include <QHelpEvent>
#include <QMenu>
#include <QToolTip>

BasicTreeView::BasicTreeView(AppEventBus* xEventBus, QWidget* xParent)
    : QTreeView(xParent)
{
    Q_UNUSED(xEventBus)
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
    for (const QPersistentModelIndex& lIndex : lIndexes) {
        const QModelIndex xIndex = lIndex;
        if (xIndex.isValid()) {
            expand(xIndex);
        }
    }
}

void BasicTreeView::mRunCollapseLoaded()
{
    QList<QPersistentModelIndex> lIndexes = mCollectLoadedIndexes();
    std::reverse(lIndexes.begin(), lIndexes.end());
    for (const QPersistentModelIndex& lIndex : lIndexes) {
        const QModelIndex xIndex = lIndex;
        if (xIndex.isValid()) {
            collapse(xIndex);
        }
    }
}

void BasicTreeView::mRunFetchCurrentBranch()
{
    const QPersistentModelIndex lCurrent = currentIndex();
    if (!lCurrent.isValid() || model() == nullptr || !model()->canFetchMore(lCurrent)) {
        return;
    }

    model()->fetchMore(lCurrent);
    expand(lCurrent);
}

void BasicTreeView::mOnNodeExpanded(const QModelIndex& xIndex)
{
    if (model() != nullptr && model()->canFetchMore(xIndex)) {
        model()->fetchMore(xIndex);
    }
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
