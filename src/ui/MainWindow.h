#pragma once

#include <QMainWindow>

#include "AppObserverInterface.h"

class AssertionView;
class DesignView;
class SignalView;
class TransactionView;
class DashboardWidget;
class ExportStrategyInterface;
class GuiMgr;
class QLabel;
class QListWidget;
class QMenu;
class QProgressBar;
class QStackedWidget;
class ServiceManager;

class MainWindow : public QMainWindow, public AppObserverInterface
{
    Q_OBJECT

public:
    explicit MainWindow(ServiceManager* xServices, QWidget* xParent = nullptr);
    ~MainWindow() override;

    void mOnProgressChanged(const QString& xTaskName, int xProgress) override;
    void mOnBusyStateChanged(const QString& xTaskName, bool xBusy) override;
    void mOnDatasetChanged() override;

private slots:
    void mRefreshDashboard();
    void mRebuildDemoData();
    void mExportCurrentView(ExportStrategyInterface* xStrategy);
    void mApplyTheme(const QString& xThemeId);

private:
    void mCreateGui();
    QLabel* mCreateBrandLabel();
    void mCreateSidebar();
    void mCreateStatusWidgets();
    void mCreateMenus();
    int mCurrentViewIndex() const;

    ServiceManager* dServices;
    GuiMgr* dGuiMgr;
    QListWidget* dSidebar;
    QStackedWidget* dViewStack;
    DashboardWidget* dDashboard;
    QLabel* dStatusLabel;
    QProgressBar* dProgressBar;
};
