#include "CsvExportStrategy.h"

#include <QIODevice>
#include <QTextStream>

QString CsvExportStrategy::mFormatName() const
{
    return QStringLiteral("CSV");
}

QString CsvExportStrategy::mFileExtension() const
{
    return QStringLiteral("csv");
}

// RFC 4180: quote any field containing comma, double-quote, or newline.
QString CsvExportStrategy::mEscapeField(const QString& xField)
{
    if (!xField.contains(QLatin1Char(','))
        && !xField.contains(QLatin1Char('"'))
        && !xField.contains(QLatin1Char('\n'))
        && !xField.contains(QLatin1Char('\r'))) {
        return xField;
    }
    QString lEscaped = xField;
    lEscaped.replace(QLatin1String("\""), QLatin1String("\"\""));
    return QLatin1Char('"') + lEscaped + QLatin1Char('"');
}

bool CsvExportStrategy::mExport(const QStringList& xHeaders,
                                const QVector<QVector<QString>>& xRows,
                                QIODevice* xDevice) const
{
    if (xDevice == nullptr || !xDevice->isWritable()) {
        return false;
    }

    QTextStream lStream(xDevice);
    lStream.setEncoding(QStringConverter::Utf8);

    QStringList lEscapedHeaders;
    lEscapedHeaders.reserve(xHeaders.size());
    for (const QString& lH : xHeaders) {
        lEscapedHeaders.append(mEscapeField(lH));
    }
    lStream << lEscapedHeaders.join(QLatin1Char(',')) << QLatin1Char('\n');

    for (const QVector<QString>& lRow : xRows) {
        QStringList lFields;
        lFields.reserve(lRow.size());
        for (const QString& lVal : lRow) {
            lFields.append(mEscapeField(lVal));
        }
        lStream << lFields.join(QLatin1Char(',')) << QLatin1Char('\n');
    }

    return lStream.status() == QTextStream::Ok;
}
