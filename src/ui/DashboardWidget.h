#pragma once

#include <QWidget>

#include "DashboardService.h"

class QGroupBox;
class QLabel;
class QComboBox;
class QGridLayout;
class DashboardSummaryChartWidget;

class DashboardWidget : public QWidget
{
    Q_OBJECT

private:
    static QGroupBox* sCreateCard(const QString& xTitle, QLabel*& xValueLabel, QWidget* xParent = nullptr);
    QGroupBox* mCreateSummaryChartCard();
    void mCreateStatsCards(QGridLayout* xLayout);
    void mConfigureGridLayout(QGridLayout* xLayout);
    void mApplyCardPalette(QGroupBox* xCard);
    void mApplyCardPalettes();
    void mCreateGui();
    QLabel* dDesignNodesValue;
    QLabel* dSignalNodesValue;
    QLabel* dAssertionsValue;
    QLabel* dAssertionFailuresValue;
    QLabel* dTransactionsValue;
    QLabel* dOpenTransactionsValue;
    QComboBox* dChartTypeCombo;
    DashboardSummaryChartWidget* dSummaryChart;

public:
    explicit DashboardWidget(QWidget* parent = nullptr);
    void mSetStats(const DashboardStats& xStats);

protected:
    void paintEvent(QPaintEvent* xEvent) override;
};
