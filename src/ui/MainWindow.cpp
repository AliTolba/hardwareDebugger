#include "MainWindow.h"

#include "CsvExportStrategy.h"
#include "DashboardWidget.h"
#include "DataExporter.h"
#include "GuiMgr.h"
#include "HtmlExportStrategy.h"
#include "JsonExportStrategy.h"
#include "ServiceManager.h"
#include "ThemeManager.h"
#include "TransactionView.h"
#include "TxtExportStrategy.h"

#include <QAction>
#include <QAbstractItemModel>
#include <QApplication>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QStackedWidget>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(ServiceManager* xServices, QWidget* xParent)
    : QMainWindow(xParent)
    , dServices(xServices)
    , dGuiMgr(nullptr)
    , dSidebar(nullptr)
    , dViewStack(nullptr)
    , dDashboard(nullptr)
    , dStatusLabel(nullptr)
    , dProgressBar(nullptr)
{
    setWindowTitle(QStringLiteral("Hardware Simulation Debugger"));
    resize(1440, 900);

    dServices->mEventBus()->mAttach(this);
    dGuiMgr = new GuiMgr(dServices, this);

    mCreateGui();
    mCreateMenus();
    dGuiMgr->mResetModels(this);
    mRefreshDashboard();

    statusBar()->showMessage(QStringLiteral("Database: %1").arg(dServices->mDatabase()->mDatabasePath()), 8000);
}

MainWindow::~MainWindow()
{
    if (dServices != nullptr) {
        dServices->mEventBus()->mDetach(this);
    }
}

void MainWindow::mOnProgressChanged(const QString& xTaskName, int xProgress)
{
    dStatusLabel->setText(xTaskName);
    if (xProgress < 0) {
        dProgressBar->setRange(0, 0);
        return;
    }

    if (dProgressBar->minimum() == 0 && dProgressBar->maximum() == 0) {
        dProgressBar->setRange(0, 100);
    }

    dProgressBar->setValue(xProgress);
}

void MainWindow::mOnBusyStateChanged(const QString& xTaskName, bool xBusy)
{
    dStatusLabel->setText(xTaskName);
    dProgressBar->setVisible(xBusy);
    if (!xBusy) {
        dProgressBar->setRange(0, 100);
        dProgressBar->setValue(0);
    }
}

void MainWindow::mOnDatasetChanged()
{
    dGuiMgr->mResetModels(this);
    mRefreshDashboard();
}

void MainWindow::mRefreshDashboard()
{
    dDashboard->mSetStats(dServices->mDashboardService()->mStats());
}

void MainWindow::mRebuildDemoData()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    const bool lOk = dServices->mRebuildDemoData();
    QApplication::restoreOverrideCursor();

    if (!lOk) {
        QMessageBox::warning(this, QStringLiteral("Rebuild failed"), QStringLiteral("Failed to recreate synthetic demo data."));
    }
}

void MainWindow::mCreateGui()
{
    auto* lCentral = new QWidget(this);
    auto* lRootLayout = new QHBoxLayout(lCentral);
    lRootLayout->setContentsMargins(12, 12, 12, 12);
    lRootLayout->setSpacing(12);

    auto* lBrandLabel = mCreateBrandLabel();
    mCreateSidebar();

    auto* lBrandLayout = new QVBoxLayout();
    lBrandLayout->addWidget(lBrandLabel);
    lBrandLayout->addWidget(dSidebar);

    auto* lMainContentLayout = new QVBoxLayout();
    lMainContentLayout->setContentsMargins(0, 0, 0, 0);
    lMainContentLayout->setSpacing(12);

    dDashboard = new DashboardWidget(this);
    lMainContentLayout->addWidget(dDashboard);

    dViewStack = new QStackedWidget(this);
    dGuiMgr->mCreateViews(dViewStack);

    if (dGuiMgr->mTransactionView() != nullptr) {
        connect(dGuiMgr->mTransactionView(), &TransactionView::mTransactionEdited, this, &MainWindow::mRefreshDashboard);
    }

    connect(dSidebar, &QListWidget::currentRowChanged, dViewStack, &QStackedWidget::setCurrentIndex);
    dSidebar->setCurrentRow(0);

    lMainContentLayout->addWidget(dViewStack, 1);

    lRootLayout->addLayout(lBrandLayout);
    lRootLayout->addLayout(lMainContentLayout, 1);
    setCentralWidget(lCentral);

    mCreateStatusWidgets();
}

QLabel* MainWindow::mCreateBrandLabel()
{
    auto* lBrandLabel = new QLabel(QStringLiteral("Siemens"), this);
    QFont lBrandFont = lBrandLabel->font();
    lBrandFont.setPointSize(lBrandFont.pointSize() + 12);
    lBrandFont.setBold(true);
    lBrandLabel->setFont(lBrandFont);
    lBrandLabel->setContentsMargins(12, 0, 12, 0);
    return lBrandLabel;
}

void MainWindow::mCreateSidebar()
{
    dSidebar = new QListWidget(this);
    dSidebar->setFixedWidth(220);
    dSidebar->setSpacing(4);
    dSidebar->addItem(QStringLiteral("Design Data"));
    dSidebar->addItem(QStringLiteral("Signal Hierarchy"));
    dSidebar->addItem(QStringLiteral("Assertion Results"));
    dSidebar->addItem(QStringLiteral("Bus Transactions"));
}

void MainWindow::mCreateStatusWidgets()
{
    dStatusLabel = new QLabel(QStringLiteral("Ready"), this);
    dProgressBar = new QProgressBar(this);
    dProgressBar->setRange(0, 100);
    dProgressBar->setFixedWidth(220);
    dProgressBar->setVisible(false);

    statusBar()->addPermanentWidget(dStatusLabel);
    statusBar()->addPermanentWidget(dProgressBar);
}

void MainWindow::mCreateMenus()
{
    QMenu* lFileMenu = menuBar()->addMenu(QStringLiteral("File"));
    QAction* lRefreshAction = lFileMenu->addAction(QStringLiteral("Refresh Dashboard"));
    QAction* lRebuildAction = lFileMenu->addAction(QStringLiteral("Rebuild Demo Data"));
    lFileMenu->addSeparator();
    QAction* lExitAction = lFileMenu->addAction(QStringLiteral("Exit"));

    connect(lRefreshAction, &QAction::triggered, this, &MainWindow::mRefreshDashboard);
    connect(lRebuildAction, &QAction::triggered, this, &MainWindow::mRebuildDemoData);
    connect(lExitAction, &QAction::triggered, this, &QWidget::close);

    QMenu* lExportMenu = menuBar()->addMenu(QStringLiteral("Export"));
    lExportMenu->addAction(QStringLiteral("Current Table as CSV (.csv)"), this, [this] { mExportCurrentView(new CsvExportStrategy); });
    lExportMenu->addAction(QStringLiteral("Current Table as Text (.tsv)"), this, [this] { mExportCurrentView(new TxtExportStrategy); });
    lExportMenu->addAction(QStringLiteral("Current Table as JSON (.json)"), this, [this] { mExportCurrentView(new JsonExportStrategy); });
    lExportMenu->addAction(QStringLiteral("Current Table as HTML (.html)"), this, [this] { mExportCurrentView(new HtmlExportStrategy); });

    QMenu* lThemeMenu = menuBar()->addMenu(QStringLiteral("Theme"));
    for (const QString& lId : ThemeManager::sInstance().mThemeIds()) {
        lThemeMenu->addAction(lId, this, [this, lId] { mApplyTheme(lId); });
    }
}

void MainWindow::mExportCurrentView(ExportStrategyInterface* xStrategy)
{
    if (dViewStack == nullptr || xStrategy == nullptr) {
        delete xStrategy;
        return;
    }

    QAbstractItemModel* lModel = dGuiMgr->mModelForViewIndex(mCurrentViewIndex());
    if (lModel != nullptr) {
        DataExporter lExporter(xStrategy, dServices->mEventBus());
        const bool lStarted = lExporter.mExport(
            lModel,
            this,
            [this](const QString& xSavedFilePath) {
                statusBar()->showMessage(QStringLiteral("Current view exported to: %1").arg(xSavedFilePath), 10000);
            },
            [this](const QString& xErrorMessage) {
                QMessageBox::warning(this, QStringLiteral("Export failed"), xErrorMessage);
            });

        if (!lStarted) {
            statusBar()->showMessage(QStringLiteral("Export cancelled."), 4000);
        }
    }
}

void MainWindow::mApplyTheme(const QString& xThemeId)
{
    ThemeManager::sInstance().mApplyTheme(xThemeId);
}

int MainWindow::mCurrentViewIndex() const
{
    return dSidebar != nullptr ? dSidebar->currentRow() : -1;
}
