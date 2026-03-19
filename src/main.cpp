#include "ServiceManager.h"
#include "MainWindow.h"
#include "ThemeManager.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("HardwareSimulationDebugger"));
    app.setOrganizationName(QStringLiteral("InterviewSample"));

    ThemeManager::sInstance().mRestoreTheme();

    ServiceManager services;
    if (!services.mInitialize()) {
        QMessageBox::critical(nullptr, QStringLiteral("Startup error"), QStringLiteral("Failed to initialize the demo database."));
        return 1;
    }

    MainWindow window(&services);
    window.show();
    return app.exec();
}
