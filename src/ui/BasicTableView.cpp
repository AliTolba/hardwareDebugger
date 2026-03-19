#include "BasicTableView.h"

#include "BasicTableViewModel.h"

#include <QEvent>
#include <QHeaderView>
#include <QHelpEvent>
#include <QMenu>
#include <QToolTip>

BasicTableView::BasicTableView(AppEventBus* xEventBus, QWidget* xParent)
    : QTableView(xParent)
{
    Q_UNUSED(xEventBus)
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
    auto* lTableModel = qobject_cast<BasicTableViewModel*>(model());
    if (lTableModel != nullptr && lTableModel->canFetchMore(QModelIndex())) {
        lTableModel->fetchMore(QModelIndex());
    }
}

void BasicTableView::mRunFetchAll()
{
    auto* lTableModel = qobject_cast<BasicTableViewModel*>(model());
    if (lTableModel == nullptr) {
        return;
    }

    lTableModel->mFetchAllRemaining();
}

void BasicTableView::mRunResetView()
{
    auto* lTableModel = qobject_cast<BasicTableViewModel*>(model());
    if (lTableModel != nullptr) {
        lTableModel->mResetCache();
    }
}