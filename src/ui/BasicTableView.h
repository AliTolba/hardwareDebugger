#pragma once

#include <QTableView>
#include <functional>

class AppEventBus;
class QThread;

class BasicTableView : public QTableView
{
    Q_OBJECT

public:
    explicit BasicTableView(AppEventBus* xEventBus, QWidget* xParent = nullptr);
    ~BasicTableView() override;

protected:
    bool viewportEvent(QEvent* xEvent) override;

private slots:
    void mShowContextMenu(const QPoint& xPosition);
    void mRunFetchNextBatch();
    void mRunFetchAll();
    void mRunResetView();

private:
    void mRunInBackground(const QString& xTaskLabel, int xSteps, const std::function<void(int)>& xAction);
    void mCleanupThread();

    AppEventBus* dEventBus;
    QThread* dWorkerThread;
};