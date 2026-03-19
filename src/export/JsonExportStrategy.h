#pragma once

#include "ExportStrategyInterface.h"

class JsonExportStrategy : public ExportStrategyInterface
{
public:
    QString mFormatName() const override;
    QString mFileExtension() const override;
    bool mExport(const QStringList& xHeaders,
                 const QVector<QVector<QString>>& xRows,
                 QIODevice* xDevice) const override;
};
