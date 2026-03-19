#include "DashboardWidget.h"

#include "DashboardSummaryChartWidget.h"

#include <QComboBox>
#include <QFont>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QPaintEvent>
#include <QVBoxLayout>


DashboardWidget::DashboardWidget(QWidget* xParent)
    : QWidget(xParent)
    , dDesignNodesValue(nullptr)
    , dSignalNodesValue(nullptr)
    , dAssertionsValue(nullptr)
    , dAssertionFailuresValue(nullptr)
    , dTransactionsValue(nullptr)
    , dOpenTransactionsValue(nullptr)
    , dChartTypeCombo(nullptr)
    , dSummaryChart(nullptr)
{
    mCreateGui();
}

void DashboardWidget::mCreateGui()
{
    auto* lLayout = new QGridLayout(this);
    lLayout->setContentsMargins(0, 0, 0, 0);
    lLayout->setHorizontalSpacing(12);
    lLayout->setVerticalSpacing(12);

    mCreateStatsCards(lLayout);
    lLayout->addWidget(mCreateSummaryChartCard(), 0, 3, 2, 1);
    mConfigureGridLayout(lLayout);

    mApplyCardPalettes();
}

void DashboardWidget::mCreateStatsCards(QGridLayout* xLayout)
{
    if (xLayout == nullptr) {
        return;
    }

    xLayout->addWidget(sCreateCard(QStringLiteral("Design nodes"), dDesignNodesValue, this), 0, 0);
    xLayout->addWidget(sCreateCard(QStringLiteral("Signal nodes"), dSignalNodesValue, this), 0, 1);
    xLayout->addWidget(sCreateCard(QStringLiteral("Assertions"), dAssertionsValue, this), 0, 2);
    xLayout->addWidget(sCreateCard(QStringLiteral("Assertion failures"), dAssertionFailuresValue, this), 1, 0);
    xLayout->addWidget(sCreateCard(QStringLiteral("Transactions"), dTransactionsValue, this), 1, 1);
    xLayout->addWidget(sCreateCard(QStringLiteral("Open transactions"), dOpenTransactionsValue, this), 1, 2);
}

QGroupBox* DashboardWidget::mCreateSummaryChartCard()
{
    auto* lChartCard = new QGroupBox(QStringLiteral("Summary chart"), this);
    QFont lTitleFont = lChartCard->font();
    lTitleFont.setPointSize(lTitleFont.pointSize() + 4);
    lTitleFont.setBold(true);
    lChartCard->setFont(lTitleFont);
    lChartCard->setProperty("dashboardCard", true);

    auto* lChartLayout = new QVBoxLayout(lChartCard);
    auto* lChartHeaderLayout = new QHBoxLayout();
    lChartHeaderLayout->setContentsMargins(0, 0, 0, 0);
    lChartHeaderLayout->addWidget(new QLabel(QStringLiteral("Chart type:"), lChartCard));

    dChartTypeCombo = new QComboBox(lChartCard);
    dChartTypeCombo->addItem(QStringLiteral("Pie"));
    dChartTypeCombo->addItem(QStringLiteral("Bar"));
    lChartHeaderLayout->addWidget(dChartTypeCombo);
    lChartHeaderLayout->addStretch(1);
    lChartLayout->addLayout(lChartHeaderLayout);

    dSummaryChart = new DashboardSummaryChartWidget(lChartCard);
    lChartLayout->addWidget(dSummaryChart);

    connect(dChartTypeCombo, &QComboBox::currentIndexChanged, this, [this](int xIndex) {
        if (dSummaryChart == nullptr) {
            return;
        }

        if (xIndex == 1) {
            dSummaryChart->mSetChartType(DashboardChartType::Bar);
        } else {
            dSummaryChart->mSetChartType(DashboardChartType::Pie);
        }
    });

    return lChartCard;
}

void DashboardWidget::mConfigureGridLayout(QGridLayout* xLayout)
{
    if (xLayout == nullptr) {
        return;
    }

    xLayout->setColumnStretch(0, 3);
    xLayout->setColumnStretch(1, 3);
    xLayout->setColumnStretch(2, 3);
    xLayout->setColumnStretch(3, 5);
}

QGroupBox* DashboardWidget::sCreateCard(const QString& xTitle, QLabel*& xValueLabel, QWidget* xParent)
{
    auto* lCard = new QGroupBox(xTitle, xParent);
    lCard->setProperty("dashboardCard", true);

    QFont lTitleFont = lCard->font();
    lTitleFont.setPointSize(lTitleFont.pointSize() + 4);
    lTitleFont.setBold(true);
    lCard->setFont(lTitleFont);

    auto* lLayout = new QVBoxLayout(lCard);
    lLayout->setContentsMargins(10, 14, 10, 10);
    xValueLabel = new QLabel(QStringLiteral("0"), lCard);
    xValueLabel->setAlignment(Qt::AlignCenter);
    QFont lValueFont = xValueLabel->font();
    lValueFont.setPointSize(lValueFont.pointSize() + 10);
    lValueFont.setBold(true);
    xValueLabel->setFont(lValueFont);
    lLayout->addWidget(xValueLabel);
    return lCard;
}

void DashboardWidget::mApplyCardPalette(QGroupBox* xCard)
{
    if (xCard == nullptr) {
        return;
    }

    const QColor lCardBackgroundColor = palette().color(QPalette::Window).darker(110);

    QPalette lCardPalette = xCard->palette();
    const QColor lCurrentWindowColor = lCardPalette.color(QPalette::Window);
    if (lCurrentWindowColor != lCardBackgroundColor)
    {
        lCardPalette.setColor(QPalette::Window, lCardBackgroundColor);
        lCardPalette.setColor(QPalette::Inactive, QPalette::Window, lCardBackgroundColor);
        xCard->setPalette(lCardPalette);
    }

    if (!xCard->autoFillBackground()) {
        xCard->setAutoFillBackground(true);
    }
}

void DashboardWidget::mApplyCardPalettes()
{
    const QList<QGroupBox*> lCards = findChildren<QGroupBox*>();
    for (QGroupBox* lCard : lCards)
    {
        mApplyCardPalette(lCard);
    }
}

void DashboardWidget::paintEvent(QPaintEvent* xEvent)
{
    mApplyCardPalettes();
    QWidget::paintEvent(xEvent);
}

void DashboardWidget::mSetStats(const DashboardStats& xStats)
{
    dDesignNodesValue->setText(QString::number(xStats.designNodes));
    dSignalNodesValue->setText(QString::number(xStats.signalNodes));
    dAssertionsValue->setText(QString::number(xStats.assertions));
    dAssertionFailuresValue->setText(QString::number(xStats.assertionFailures));
    dTransactionsValue->setText(QString::number(xStats.transactions));
    dOpenTransactionsValue->setText(QString::number(xStats.openTransactions));
    if (dSummaryChart != nullptr)
    {
        dSummaryChart->mSetStats(xStats);
    }
}
