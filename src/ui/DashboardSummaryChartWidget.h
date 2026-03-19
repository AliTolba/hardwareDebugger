#pragma once

#include <QWidget>

#include "DashboardService.h"

class QChartView;
class QChart;

enum class DashboardChartType
{
    Pie,
    Bar
};

class DashboardSummaryChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardSummaryChartWidget(QWidget* xParent = nullptr);

    void mSetStats(const DashboardStats& xStats);
    void mSetChartType(DashboardChartType xChartType);

protected:
    void changeEvent(QEvent* xEvent) override;

private:
    void mRebuildChart();
    QChart* mBuildPieChart(const QVector<QColor>& xColors);
    QChart* mBuildBarChart(const QVector<QColor>& xColors);

    DashboardStats dStats;
    DashboardChartType dChartType;
    QChartView* dChartView;
};
