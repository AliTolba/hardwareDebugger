#include "MainWindow.h"

#include "ServiceManager.h"
#include "BasicTableViewModel.h"
#include "BasicTreeViewModel.h"
#include "TransactionTableViewModel.h"
#include "AssertionView.h"
#include "DesignView.h"
#include "SignalView.h"
#include "TransactionView.h"
#include "DashboardWidget.h"
#include "DataExporter.h"
#include "CsvExportStrategy.h"
#include "TxtExportStrategy.h"
#include "JsonExportStrategy.h"
#include "HtmlExportStrategy.h"
#include "ThemeManager.h"

#include <QAction>
#include <QAbstractItemModel>
#include <QApplication>
#include <QFont>
#include <QHeaderView>
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
    , dSidebar(nullptr)
    , dViewStack(nullptr)
    , dDashboard(nullptr)
    , dStatusLabel(nullptr)
    , dProgressBar(nullptr)
    , dDesignView(nullptr)
    , dSignalView(nullptr)
    , dAssertionView(nullptr)
    , dTransactionView(nullptr)
    , dDesignModel(nullptr)
    , dSignalModel(nullptr)
    , dAssertionModel(nullptr)
    , dTransactionModel(nullptr)
{
    setWindowTitle(QStringLiteral("Hardware Simulation Debugger"));
    resize(1440, 900);

    dServices->mEventBus()->mAttach(this);

    mCreateGui();
    mCreateMenus();
    mResetModels();
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
    dProgressBar->setValue(xProgress);
}

void MainWindow::mOnBusyStateChanged(const QString& xTaskName, bool xBusy)
{
    dStatusLabel->setText(xTaskName);
    dProgressBar->setVisible(xBusy);
    if (!xBusy) {
        dProgressBar->setValue(0);
    }
}

void MainWindow::mOnDatasetChanged()
{
    mResetModels();
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

    mCreateViewStack();

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

void MainWindow::mCreateViewStack()
{
    dViewStack = new QStackedWidget(this);
    dDesignView = new DesignView(dServices->mEventBus(), dViewStack);
    dSignalView = new SignalView(dServices->mEventBus(), dViewStack);
    dAssertionView = new AssertionView(dServices->mEventBus(), dViewStack);
    dTransactionView = new TransactionView(dServices->mEventBus(), dViewStack);
    connect(dTransactionView, &TransactionView::mTransactionEdited, this, &MainWindow::mRefreshDashboard);

    dViewStack->addWidget(dDesignView);
    dViewStack->addWidget(dSignalView);
    dViewStack->addWidget(dAssertionView);
    dViewStack->addWidget(dTransactionView);
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

    QAbstractItemModel* lModel = nullptr;
    switch (mCurrentViewIndex()) {
        case 0: lModel = dDesignModel;      break;
        case 1: lModel = dSignalModel;      break;
        case 2: lModel = dAssertionModel;   break;
        case 3: lModel = dTransactionModel; break;
        default: break;
    }

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

void MainWindow::mResetModels()
{
    delete dDesignModel;
    delete dSignalModel;
    delete dAssertionModel;
    delete dTransactionModel;

    dDesignModel = new BasicTreeViewModel(dServices->mDesignViewService(), 32, this);
    dSignalModel = new BasicTreeViewModel(dServices->mSignalViewService(), 32, this);
    dAssertionModel = new BasicTableViewModel(dServices->mAssertionViewService(), 400, this);
    dTransactionModel = new TransactionTableViewModel(dServices->mTransactionViewService(), 400, this);

    dDesignView->setModel(dDesignModel);
    dSignalView->setModel(dSignalModel);
    dAssertionView->setModel(dAssertionModel);
    dTransactionView->setModel(dTransactionModel);

    dDesignView->header()->setStretchLastSection(true);
    dSignalView->header()->setStretchLastSection(true);
    dAssertionView->resizeColumnsToContents();
    dTransactionView->resizeColumnsToContents();
}

int MainWindow::mCurrentViewIndex() const
{
    return dSidebar != nullptr ? dSidebar->currentRow() : -1;
}
