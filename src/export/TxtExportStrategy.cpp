#include "TxtExportStrategy.h"

#include <QIODevice>
#include <QTextStream>

QString TxtExportStrategy::mFormatName() const
{
    return QStringLiteral("Text (TSV)");
}

QString TxtExportStrategy::mFileExtension() const
{
    return QStringLiteral("tsv");
}

bool TxtExportStrategy::mExport(const QStringList& xHeaders,
                                const QVector<QVector<QString>>& xRows,
                                QIODevice* xDevice) const
{
    if (xDevice == nullptr || !xDevice->isWritable()) {
        return false;
    }

    QTextStream lStream(xDevice);
    lStream.setEncoding(QStringConverter::Utf8);

    lStream << xHeaders.join(QLatin1Char('\t')) << QLatin1Char('\n');

    for (const QVector<QString>& lRow : xRows) {
        QStringList lFields;
        lFields.reserve(lRow.size());
        for (const QString& lVal : lRow) {
            // Replace any embedded tabs/newlines to keep columns intact.
            QString lSafe = lVal;
            lSafe.replace(QLatin1Char('\t'), QLatin1String("    "));
            lSafe.replace(QLatin1Char('\n'), QLatin1String(" "));
            lFields.append(lSafe);
        }
        lStream << lFields.join(QLatin1Char('\t')) << QLatin1Char('\n');
    }

    return lStream.status() == QTextStream::Ok;
}
