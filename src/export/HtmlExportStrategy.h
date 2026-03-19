#pragma once

#include "ExportStrategyInterface.h"

class HtmlExportStrategy : public ExportStrategyInterface
{
public:
    QString mFormatName() const override;
    QString mFileExtension() const override;
    bool mExport(const QStringList& xHeaders,
                 const QVector<QVector<QString>>& xRows,
                 QIODevice* xDevice) const override;

private:
    static QString mEscapeHtml(const QString& xText);
};
