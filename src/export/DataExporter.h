#pragma once

#include <functional>
#include <memory>
#include <QStringList>
#include <QVector>

class ExportStrategyInterface;
class AppEventBus;
class QAbstractItemModel;
class QWidget;

class DataExporter
{
public:
    // Takes ownership of xStrategy.
    explicit DataExporter(ExportStrategyInterface* xStrategy, AppEventBus* xEventBus = nullptr);

    // Shows a save-file dialog, then exports all visible data from xModel.
    // Tree models are flattened depth-first; depth is reflected via indentation
    // on the first column. The file write runs on a worker thread and this
    // method returns immediately after scheduling that work.
    // Returns false only when export could not be started.
    bool mExport(QAbstractItemModel* xModel,
                 QWidget* xParent = nullptr,
                 const std::function<void(const QString&)>& xOnSuccess = {},
                 const std::function<void(const QString&)>& xOnError = {}) const;

private:
    std::shared_ptr<ExportStrategyInterface> dStrategy;
    AppEventBus* dEventBus;
};
