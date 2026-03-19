#pragma once

#include <QStringList>
#include <QVector>

class QIODevice;

class ExportStrategyInterface
{
public:
    virtual ~ExportStrategyInterface() = default;

    virtual QString mFormatName() const = 0;
    virtual QString mFileExtension() const = 0;

    // xHeaders: column names; xRows: all data rows as strings (already flattened)
    virtual bool mExport(const QStringList& xHeaders,
                         const QVector<QVector<QString>>& xRows,
                         QIODevice* xDevice) const = 0;
};
