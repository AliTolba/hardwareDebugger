#pragma once

#include <QObject>
#include <QString>

class QSqlDatabase;

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject* xParent = nullptr);
    ~DatabaseManager() override;

    bool mInitialize();
    bool mReseed();

    QString mDatabasePath() const;
    QSqlDatabase mConnection() const;

private:
    bool mOpen();
    bool mCreateSchema();
    bool mRecreateSchema();
    bool mPopulateIfEmpty();
    bool mClearAllData();
    bool mPopulateDesignTable();
    bool mPopulateSignalTable();
    bool mPopulateAssertionTable();
    bool mPopulateTransactionTable();
    int mTableCount(const QString& xTableName) const;
    QSqlDatabase mDatabase() const;

    QString dConnectionName;
    QString dDatabasePath;
};
