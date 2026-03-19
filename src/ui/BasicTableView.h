#pragma once

#include <QTableView>

class AppEventBus;

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
};