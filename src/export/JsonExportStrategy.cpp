#include "JsonExportStrategy.h"

#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

QString JsonExportStrategy::mFormatName() const
{
    return QStringLiteral("JSON");
}

QString JsonExportStrategy::mFileExtension() const
{
    return QStringLiteral("json");
}

bool JsonExportStrategy::mExport(const QStringList& xHeaders,
                                 const QVector<QVector<QString>>& xRows,
                                 QIODevice* xDevice) const
{
    if (xDevice == nullptr || !xDevice->isWritable()) {
        return false;
    }

    QJsonArray lArray;
    for (const QVector<QString>& lRow : xRows) {
        QJsonObject lObj;
        for (int lCol = 0; lCol < xHeaders.size() && lCol < lRow.size(); ++lCol) {
            lObj.insert(xHeaders.at(lCol), lRow.at(lCol));
        }
        lArray.append(lObj);
    }

    const QByteArray lBytes = QJsonDocument(lArray).toJson(QJsonDocument::Indented);
    return xDevice->write(lBytes) == lBytes.size();
}
