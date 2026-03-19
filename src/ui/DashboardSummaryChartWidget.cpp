#include "DashboardSummaryChartWidget.h"

#include <QCursor>
#include <QEvent>
#include <QPalette>
#include <QToolTip>
#include <QVBoxLayout>
#include <QVector>

#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QValueAxis>

namespace
{
struct ChartItem
{
    QString label;
    int value;
};

QVector<ChartItem> sBuildItems(const DashboardStats& xStats)
{
    return {
        {QStringLiteral("Design"), xStats.designNodes},
        {QStringLiteral("Signals"), xStats.signalNodes},
        {QStringLiteral("Assertions"), xStats.assertions},
        {QStringLiteral("Failures"), xStats.assertionFailures},
        {QStringLiteral("Transactions"), xStats.transactions},
        {QStringLiteral("Open Tx"), xStats.openTransactions}};
}

QVector<QColor> sBuildColors(const QColor& xBaseColor)
{
    return {
        xBaseColor,
        xBaseColor.lighter(120),
        xBaseColor.darker(110),
        xBaseColor.lighter(140),
        xBaseColor.darker(130),
        xBaseColor.lighter(160)};
}

void sApplyChartTheme(QChart* xChart, const QPalette& xPalette)
{
    if (xChart == nullptr) {
        return;
    }

    xChart->setBackgroundVisible(false);
    xChart->setPlotAreaBackgroundVisible(false);
    xChart->setAnimationOptions(QChart::SeriesAnimations);

    QBrush lTitleBrush(xPalette.color(QPalette::WindowText));
    xChart->setTitleBrush(lTitleBrush);

    QPen lAxisPen(xPalette.color(QPalette::Mid));
    xChart->legend()->setLabelColor(xPalette.color(QPalette::WindowText));

    for (QAbstractAxis* lAxis : xChart->axes()) {
        lAxis->setLabelsBrush(lTitleBrush);
        lAxis->setLinePen(lAxisPen);
        lAxis->setGridLinePen(QPen(xPalette.color(QPalette::Mid)));
    }
}
}

DashboardSummaryChartWidget::DashboardSummaryChartWidget(QWidget* xParent)
    : QWidget(xParent)
    , dChartType(DashboardChartType::Pie)
    , dChartView(nullptr)
{
    setMinimumHeight(260);

    auto* lLayout = new QVBoxLayout(this);
    lLayout->setContentsMargins(0, 0, 0, 0);

    dChartView = new QChartView(this);
    dChartView->setRenderHint(QPainter::Antialiasing, true);
    dChartView->setRubberBand(QChartView::NoRubberBand);
    lLayout->addWidget(dChartView);

    mRebuildChart();
}

void DashboardSummaryChartWidget::mSetStats(const DashboardStats& xStats)
{
    dStats = xStats;
    mRebuildChart();
}

void DashboardSummaryChartWidget::mSetChartType(DashboardChartType xChartType)
{
    dChartType = xChartType;
    mRebuildChart();
}

void DashboardSummaryChartWidget::changeEvent(QEvent* xEvent)
{
    QWidget::changeEvent(xEvent);

    if (xEvent != nullptr
        && (xEvent->type() == QEvent::PaletteChange || xEvent->type() == QEvent::ApplicationPaletteChange))
    {
        mRebuildChart();
    }
}

void DashboardSummaryChartWidget::mRebuildChart()
{
    if (dChartView == nullptr) {
        return;
    }

    const QVector<QColor> lColors = sBuildColors(palette().highlight().color());
    QChart* lChart = nullptr;
    if (dChartType == DashboardChartType::Pie) {
        lChart = mBuildPieChart(lColors);
    } else {
        lChart = mBuildBarChart(lColors);
    }

    sApplyChartTheme(lChart, palette());
    dChartView->setChart(lChart);
}

QChart* DashboardSummaryChartWidget::mBuildPieChart(const QVector<QColor>& xColors)
{
    const QVector<ChartItem> lItems = sBuildItems(dStats);
    const QColor lLabelColor = palette().color(QPalette::WindowText);

    auto* lChart = new QChart();
    lChart->setTitle(QStringLiteral("Summary"));

    auto* lSeries = new QPieSeries(lChart);

    int lTotal = 0;
    for (const ChartItem& lItem : lItems) {
        lTotal += qMax(0, lItem.value);
    }
    const bool lUseFallback = (lTotal <= 0);

    for (int lIndex = 0; lIndex < lItems.size(); ++lIndex)
    {
        const int lValue = lUseFallback ? 1 : qMax(0, lItems[lIndex].value);
        QPieSlice* lSlice = lSeries->append(lItems[lIndex].label, lValue);
        lSlice->setBrush(xColors[lIndex % xColors.size()]);
        lSlice->setLabelColor(lLabelColor);

        const QString lTooltip = QStringLiteral("%1: %2")
            .arg(lItems[lIndex].label, QString::number(lItems[lIndex].value));
        connect(lSlice, &QPieSlice::hovered, this, [lTooltip](bool xHovered) {
            if (xHovered) {
                QToolTip::showText(QCursor::pos(), lTooltip);
            } else {
                QToolTip::hideText();
            }
        });
    }

    lSeries->setLabelsVisible(true);
    lChart->addSeries(lSeries);
    lChart->legend()->setVisible(true);

    return lChart;
}

QChart* DashboardSummaryChartWidget::mBuildBarChart(const QVector<QColor>& xColors)
{
    const QVector<ChartItem> lItems = sBuildItems(dStats);

    auto* lChart = new QChart();
    lChart->setTitle(QStringLiteral("Summary"));

    auto* lSeries = new QBarSeries(lChart);
    auto* lSet = new QBarSet(QStringLiteral("Count"), lSeries);

    QStringList lCategories;
    int lMaxValue = 1;
    for (const ChartItem& lItem : lItems)
    {
        *lSet << qMax(0, lItem.value);
        lCategories << lItem.label;
        lMaxValue = qMax(lMaxValue, qMax(0, lItem.value));
    }

    if (!xColors.isEmpty()) {
        lSet->setColor(xColors.front());
    } else {
        lSet->setColor(palette().highlight().color());
    }
    lSeries->append(lSet);
    lChart->addSeries(lSeries);

    auto* lAxisX = new QBarCategoryAxis(lChart);
    lAxisX->append(lCategories);
    lChart->addAxis(lAxisX, Qt::AlignBottom);
    lSeries->attachAxis(lAxisX);

    auto* lAxisY = new QValueAxis(lChart);
    lAxisY->setRange(0, qMax(1, lMaxValue));
    lAxisY->setLabelFormat(QStringLiteral("%d"));
    lChart->addAxis(lAxisY, Qt::AlignLeft);
    lSeries->attachAxis(lAxisY);

    connect(lSet, &QBarSet::hovered, this, [lItems](bool xHovered, int xIndex) {
        if (!xHovered || xIndex < 0 || xIndex >= lItems.size()) {
            QToolTip::hideText();
            return;
        }

        const QString lTooltip = QStringLiteral("%1: %2")
            .arg(lItems[xIndex].label, QString::number(lItems[xIndex].value));
        QToolTip::showText(QCursor::pos(), lTooltip);
    });

    lChart->legend()->setVisible(false);

    return lChart;
}
