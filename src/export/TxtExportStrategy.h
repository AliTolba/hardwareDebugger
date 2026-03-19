#pragma once

#include "ExportStrategyInterface.h"

class TxtExportStrategy : public ExportStrategyInterface
{
public:
    QString mFormatName() const override;
    QString mFileExtension() const override;
    bool mExport(const QStringList& xHeaders,
                 const QVector<QVector<QString>>& xRows,
                 QIODevice* xDevice) const override;
};
