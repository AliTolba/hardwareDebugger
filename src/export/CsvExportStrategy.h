#pragma once

#include "ExportStrategyInterface.h"

class CsvExportStrategy : public ExportStrategyInterface
{
public:
    QString mFormatName() const override;
    QString mFileExtension() const override;
    bool mExport(const QStringList& xHeaders,
                 const QVector<QVector<QString>>& xRows,
                 QIODevice* xDevice) const override;

private:
    static QString mEscapeField(const QString& xField);
};
