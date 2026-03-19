#include "HtmlExportStrategy.h"

#include <QIODevice>
#include <QTextStream>

QString HtmlExportStrategy::mFormatName() const
{
    return QStringLiteral("HTML");
}

QString HtmlExportStrategy::mFileExtension() const
{
    return QStringLiteral("html");
}

QString HtmlExportStrategy::mEscapeHtml(const QString& xText)
{
    QString lOut = xText;
    lOut.replace(QLatin1Char('&'),  QLatin1String("&amp;"));
    lOut.replace(QLatin1Char('<'),  QLatin1String("&lt;"));
    lOut.replace(QLatin1Char('>'),  QLatin1String("&gt;"));
    lOut.replace(QLatin1Char('"'),  QLatin1String("&quot;"));
    return lOut;
}

bool HtmlExportStrategy::mExport(const QStringList& xHeaders,
                                 const QVector<QVector<QString>>& xRows,
                                 QIODevice* xDevice) const
{
    if (xDevice == nullptr || !xDevice->isWritable()) {
        return false;
    }

    QTextStream lStream(xDevice);
    lStream.setEncoding(QStringConverter::Utf8);

    lStream << QStringLiteral("<!DOCTYPE html>\n<html>\n<head>\n")
            << QStringLiteral("<meta charset=\"utf-8\">\n")
            << QStringLiteral("<style>\n")
            << QStringLiteral("table{border-collapse:collapse;font-family:monospace;font-size:13px}\n")
            << QStringLiteral("th,td{border:1px solid #ccc;padding:4px 8px}\n")
            << QStringLiteral("th{background:#f0f0f0}\n")
            << QStringLiteral("tr:nth-child(even){background:#f9f9f9}\n")
            << QStringLiteral("</style>\n</head>\n<body>\n<table>\n<thead>\n<tr>\n");

    for (const QString& lH : xHeaders) {
        lStream << QStringLiteral("<th>") << mEscapeHtml(lH) << QStringLiteral("</th>\n");
    }
    lStream << QStringLiteral("</tr>\n</thead>\n<tbody>\n");

    for (const QVector<QString>& lRow : xRows) {
        lStream << QStringLiteral("<tr>\n");
        for (const QString& lVal : lRow) {
            lStream << QStringLiteral("<td>") << mEscapeHtml(lVal) << QStringLiteral("</td>\n");
        }
        lStream << QStringLiteral("</tr>\n");
    }

    lStream << QStringLiteral("</tbody>\n</table>\n</body>\n</html>\n");
    return lStream.status() == QTextStream::Ok;
}
