#include "DatabaseManager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QRandomGenerator>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QUuid>

namespace
{
constexpr int kSchemaVersion = 2;

QString randomFrom(const QStringList& values)
{
    const int index = QRandomGenerator::global()->bounded(values.size());
    return values.at(index);
}

bool execOrFail(QSqlQuery& query, const QString& sql)
{
    if (!query.exec(sql)) {
        qWarning("SQL error: %s", qPrintable(query.lastError().text()));
        return false;
    }

    return true;
}

int readUserVersion(const QSqlDatabase& db)
{
    QSqlQuery query(db);
    if (!query.exec(QStringLiteral("PRAGMA user_version")) || !query.next()) {
        return 0;
    }

    return query.value(0).toInt();
}
}

DatabaseManager::DatabaseManager(QObject* xParent)
    : QObject(xParent)
    , dConnectionName(QStringLiteral("hardware_debugger_%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces)))
{
}

DatabaseManager::~DatabaseManager()
{
    if (QSqlDatabase::contains(dConnectionName)) {
        QSqlDatabase lDb = QSqlDatabase::database(dConnectionName);
        lDb.close();
        QSqlDatabase::removeDatabase(dConnectionName);
    }
}

bool DatabaseManager::mInitialize()
{
    return mOpen() && mCreateSchema() && mPopulateIfEmpty();
}

bool DatabaseManager::mReseed()
{
    return mOpen() && mCreateSchema() && mClearAllData() && mPopulateIfEmpty();
}

QString DatabaseManager::mDatabasePath() const
{
    return dDatabasePath;
}

QSqlDatabase DatabaseManager::mConnection() const
{
    return mDatabase();
}

bool DatabaseManager::mOpen()
{
    if (QSqlDatabase::contains(dConnectionName) && QSqlDatabase::database(dConnectionName).isOpen()) {
        return true;
    }

    QString lBasePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (lBasePath.isEmpty()) {
        lBasePath = QCoreApplication::applicationDirPath();
    }

    QDir().mkpath(lBasePath);
    dDatabasePath = QDir(lBasePath).filePath(QStringLiteral("hardware_debugger_demo.sqlite"));

    QSqlDatabase lDb = QSqlDatabase::contains(dConnectionName)
        ? QSqlDatabase::database(dConnectionName)
        : QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), dConnectionName);
    lDb.setDatabaseName(dDatabasePath);

    if (!lDb.open()) {
        qWarning("Failed to open database: %s", qPrintable(lDb.lastError().text()));
        return false;
    }

    return true;
}

bool DatabaseManager::mCreateSchema()
{
    QSqlDatabase lDb = mDatabase();
    if (readUserVersion(lDb) == kSchemaVersion) {
        return true;
    }

    return mRecreateSchema();
}

bool DatabaseManager::mRecreateSchema()
{
    QSqlDatabase lDb = mDatabase();
    QSqlQuery lQuery(lDb);

    const bool lOk = execOrFail(lQuery, QStringLiteral("DROP TABLE IF EXISTS design_nodes"))
        && execOrFail(lQuery, QStringLiteral("DROP TABLE IF EXISTS signal_nodes"))
        && execOrFail(lQuery, QStringLiteral("DROP TABLE IF EXISTS assertions"))
        && execOrFail(lQuery, QStringLiteral("DROP TABLE IF EXISTS transactions"))
        && execOrFail(lQuery, QStringLiteral(
            "CREATE TABLE design_nodes ("
            "id INTEGER PRIMARY KEY, "
            "parent_id INTEGER NULL, "
            "instance_name TEXT NOT NULL, "
            "node_kind TEXT NOT NULL, "
            "source_file TEXT NOT NULL, "
            "child_count INTEGER NOT NULL DEFAULT 0)"))
        && execOrFail(lQuery, QStringLiteral(
            "CREATE TABLE signal_nodes ("
            "id INTEGER PRIMARY KEY, "
            "parent_id INTEGER NULL, "
            "signal_name TEXT NOT NULL, "
            "direction TEXT NOT NULL, "
            "width_bits INTEGER NOT NULL, "
            "domain_name TEXT NOT NULL, "
            "child_count INTEGER NOT NULL DEFAULT 0)"))
        && execOrFail(lQuery, QStringLiteral(
            "CREATE TABLE assertions ("
            "id INTEGER PRIMARY KEY, "
            "property_name TEXT NOT NULL, "
            "module_name TEXT NOT NULL, "
            "severity TEXT NOT NULL, "
            "status TEXT NOT NULL, "
            "message TEXT NOT NULL, "
            "cycle INTEGER NOT NULL, "
            "time_ns INTEGER NOT NULL)"))
        && execOrFail(lQuery, QStringLiteral(
            "CREATE TABLE transactions ("
            "id INTEGER PRIMARY KEY, "
            "channel TEXT NOT NULL, "
            "operation TEXT NOT NULL, "
            "address INTEGER NOT NULL, "
            "burst_length INTEGER NOT NULL, "
            "response_code TEXT NOT NULL, "
            "cycle INTEGER NOT NULL, "
            "status TEXT NOT NULL)"))
        && execOrFail(lQuery, QStringLiteral("CREATE INDEX idx_design_parent ON design_nodes(parent_id)"))
        && execOrFail(lQuery, QStringLiteral("CREATE INDEX idx_signal_parent ON signal_nodes(parent_id)"))
        && execOrFail(lQuery, QStringLiteral("CREATE INDEX idx_assertion_status ON assertions(status)"))
        && execOrFail(lQuery, QStringLiteral("CREATE INDEX idx_transaction_status ON transactions(status)"))
        && execOrFail(lQuery, QStringLiteral("PRAGMA user_version = 2"));

    return lOk;
}

bool DatabaseManager::mPopulateIfEmpty()
{
    if (mTableCount(QStringLiteral("design_nodes")) > 0) {
        return true;
    }

    return mPopulateDesignTable()
        && mPopulateSignalTable()
        && mPopulateAssertionTable()
        && mPopulateTransactionTable();
}

bool DatabaseManager::mClearAllData()
{
    QSqlDatabase lDb = mDatabase();
    if (!lDb.transaction()) {
        return false;
    }

    QSqlQuery lQuery(lDb);
    const bool lOk = execOrFail(lQuery, QStringLiteral("DELETE FROM design_nodes"))
        && execOrFail(lQuery, QStringLiteral("DELETE FROM signal_nodes"))
        && execOrFail(lQuery, QStringLiteral("DELETE FROM assertions"))
        && execOrFail(lQuery, QStringLiteral("DELETE FROM transactions"));

    return lOk ? lDb.commit() : lDb.rollback();
}

bool DatabaseManager::mPopulateDesignTable()
{
    QSqlDatabase lDb = mDatabase();
    if (!lDb.transaction()) {
        return false;
    }

    QSqlQuery lQuery(lDb);
    lQuery.prepare(QStringLiteral(
        "INSERT INTO design_nodes (id, parent_id, instance_name, node_kind, source_file, child_count) "
        "VALUES (:id, :parent_id, :instance_name, :node_kind, :source_file, :child_count)"));

    int lId = 1;
    for (int lRoot = 0; lRoot < 120; ++lRoot) {
        const int lRootId = lId++;
        lQuery.bindValue(QStringLiteral(":id"), lRootId);
        lQuery.bindValue(QStringLiteral(":parent_id"), QVariant());
        lQuery.bindValue(QStringLiteral(":instance_name"), QStringLiteral("top_%1").arg(lRoot, 3, 10, QLatin1Char('0')));
        lQuery.bindValue(QStringLiteral(":node_kind"), QStringLiteral("Top Module"));
        lQuery.bindValue(QStringLiteral(":source_file"), QStringLiteral("rtl/top_%1.sv").arg(lRoot));
        lQuery.bindValue(QStringLiteral(":child_count"), 60);
        if (!lQuery.exec()) {
            lDb.rollback();
            return false;
        }

        for (int lBranch = 0; lBranch < 60; ++lBranch) {
            const int lBranchId = lId++;
            lQuery.bindValue(QStringLiteral(":id"), lBranchId);
            lQuery.bindValue(QStringLiteral(":parent_id"), lRootId);
            lQuery.bindValue(QStringLiteral(":instance_name"), QStringLiteral("u_block_%1_%2").arg(lRoot).arg(lBranch));
            lQuery.bindValue(QStringLiteral(":node_kind"), lBranch % 2 == 0 ? QStringLiteral("Module") : QStringLiteral("Generate Block"));
            lQuery.bindValue(QStringLiteral(":source_file"), QStringLiteral("rtl/block_%1_%2.sv").arg(lRoot).arg(lBranch));
            lQuery.bindValue(QStringLiteral(":child_count"), 25);
            if (!lQuery.exec()) {
                lDb.rollback();
                return false;
            }

            for (int lLeaf = 0; lLeaf < 25; ++lLeaf) {
                lQuery.bindValue(QStringLiteral(":id"), lId++);
                lQuery.bindValue(QStringLiteral(":parent_id"), lBranchId);
                lQuery.bindValue(QStringLiteral(":instance_name"), QStringLiteral("u_inst_%1_%2_%3").arg(lRoot).arg(lBranch).arg(lLeaf));
                lQuery.bindValue(QStringLiteral(":node_kind"), lLeaf % 3 == 0 ? QStringLiteral("Always Block") : QStringLiteral("Instance"));
                lQuery.bindValue(QStringLiteral(":source_file"), QStringLiteral("rtl/leaf_%1_%2.sv").arg(lBranch).arg(lLeaf));
                lQuery.bindValue(QStringLiteral(":child_count"), 0);
                if (!lQuery.exec()) {
                    lDb.rollback();
                    return false;
                }
            }
        }
    }

    return lDb.commit();
}

bool DatabaseManager::mPopulateSignalTable()
{
    static const QStringList sDirections = {
        QStringLiteral("Input"),
        QStringLiteral("Output"),
        QStringLiteral("InOut"),
        QStringLiteral("Internal")
    };

    QSqlDatabase lDb = mDatabase();
    if (!lDb.transaction()) {
        return false;
    }

    QSqlQuery lQuery(lDb);
    lQuery.prepare(QStringLiteral(
        "INSERT INTO signal_nodes (id, parent_id, signal_name, direction, width_bits, domain_name, child_count) "
        "VALUES (:id, :parent_id, :signal_name, :direction, :width_bits, :domain_name, :child_count)"));

    int lId = 1;
    for (int lRoot = 0; lRoot < 80; ++lRoot) {
        const int lRootId = lId++;
        lQuery.bindValue(QStringLiteral(":id"), lRootId);
        lQuery.bindValue(QStringLiteral(":parent_id"), QVariant());
        lQuery.bindValue(QStringLiteral(":signal_name"), QStringLiteral("domain_%1").arg(lRoot));
        lQuery.bindValue(QStringLiteral(":direction"), QStringLiteral("Group"));
        lQuery.bindValue(QStringLiteral(":width_bits"), 0);
        lQuery.bindValue(QStringLiteral(":domain_name"), QStringLiteral("clk_%1").arg(lRoot % 4));
        lQuery.bindValue(QStringLiteral(":child_count"), 50);
        if (!lQuery.exec()) {
            lDb.rollback();
            return false;
        }

        for (int lBranch = 0; lBranch < 50; ++lBranch) {
            const int lBranchId = lId++;
            lQuery.bindValue(QStringLiteral(":id"), lBranchId);
            lQuery.bindValue(QStringLiteral(":parent_id"), lRootId);
            lQuery.bindValue(QStringLiteral(":signal_name"), QStringLiteral("bus_%1_%2").arg(lRoot).arg(lBranch));
            lQuery.bindValue(QStringLiteral(":direction"), QStringLiteral("Bundle"));
            lQuery.bindValue(QStringLiteral(":width_bits"), (lBranch % 8 + 1) * 16);
            lQuery.bindValue(QStringLiteral(":domain_name"), QStringLiteral("pd_%1").arg(lBranch % 5));
            lQuery.bindValue(QStringLiteral(":child_count"), 20);
            if (!lQuery.exec()) {
                lDb.rollback();
                return false;
            }

            for (int lLeaf = 0; lLeaf < 20; ++lLeaf) {
                lQuery.bindValue(QStringLiteral(":id"), lId++);
                lQuery.bindValue(QStringLiteral(":parent_id"), lBranchId);
                lQuery.bindValue(QStringLiteral(":signal_name"), QStringLiteral("sig_%1_%2_%3").arg(lRoot).arg(lBranch).arg(lLeaf));
                lQuery.bindValue(QStringLiteral(":direction"), randomFrom(sDirections));
                lQuery.bindValue(QStringLiteral(":width_bits"), (lLeaf % 8 + 1) * 8);
                lQuery.bindValue(QStringLiteral(":domain_name"), QStringLiteral("domain_%1").arg((lRoot + lLeaf) % 6));
                lQuery.bindValue(QStringLiteral(":child_count"), 0);
                if (!lQuery.exec()) {
                    lDb.rollback();
                    return false;
                }
            }
        }
    }

    return lDb.commit();
}

bool DatabaseManager::mPopulateAssertionTable()
{
    static const QStringList sSeverities = {QStringLiteral("Info"), QStringLiteral("Warning"), QStringLiteral("Error")};
    static const QStringList sStates = {QStringLiteral("PASS"), QStringLiteral("PASS"), QStringLiteral("PASS"), QStringLiteral("PASS"), QStringLiteral("FAIL")};

    QSqlDatabase lDb = mDatabase();
    if (!lDb.transaction()) {
        return false;
    }

    QSqlQuery lQuery(lDb);
    lQuery.prepare(QStringLiteral(
        "INSERT INTO assertions (id, property_name, module_name, severity, status, message, cycle, time_ns) "
        "VALUES (:id, :property_name, :module_name, :severity, :status, :message, :cycle, :time_ns)"));

    for (int lI = 1; lI <= 500000; ++lI) {
        const QString lStatus = randomFrom(sStates);
        lQuery.bindValue(QStringLiteral(":id"), lI);
        lQuery.bindValue(QStringLiteral(":property_name"), QStringLiteral("p_stable_%1").arg(lI % 1000));
        lQuery.bindValue(QStringLiteral(":module_name"), QStringLiteral("top_%1/mod_%2").arg(lI % 120).arg(lI % 300));
        lQuery.bindValue(QStringLiteral(":severity"), lStatus == QStringLiteral("FAIL") ? QStringLiteral("Error") : randomFrom(sSeverities));
        lQuery.bindValue(QStringLiteral(":status"), lStatus);
        lQuery.bindValue(QStringLiteral(":message"), QStringLiteral("Property %1 sampled at stage %2").arg(lI % 1000).arg(lI % 64));
        lQuery.bindValue(QStringLiteral(":cycle"), 1000 + lI * 4);
        lQuery.bindValue(QStringLiteral(":time_ns"), 5 * lI);
        if (!lQuery.exec()) {
            lDb.rollback();
            return false;
        }
    }

    return lDb.commit();
}

bool DatabaseManager::mPopulateTransactionTable()
{
    static const QStringList sChannels = {QStringLiteral("AXI-AW"), QStringLiteral("AXI-W"), QStringLiteral("AXI-B"), QStringLiteral("AXI-AR"), QStringLiteral("AXI-R")};
    static const QStringList sOperations = {QStringLiteral("Read"), QStringLiteral("Write"), QStringLiteral("Burst")};
    static const QStringList sStatuses = {QStringLiteral("Complete"), QStringLiteral("Complete"), QStringLiteral("Pending"), QStringLiteral("Retry")};
    static const QStringList sResponses = {QStringLiteral("OKAY"), QStringLiteral("EXOKAY"), QStringLiteral("SLVERR"), QStringLiteral("DECERR")};

    QSqlDatabase lDb = mDatabase();
    if (!lDb.transaction()) {
        return false;
    }

    QSqlQuery lQuery(lDb);
    lQuery.prepare(QStringLiteral(
        "INSERT INTO transactions (id, channel, operation, address, burst_length, response_code, cycle, status) "
        "VALUES (:id, :channel, :operation, :address, :burst_length, :response_code, :cycle, :status)"));

    for (int lI = 1; lI <= 1000000; ++lI) {
        lQuery.bindValue(QStringLiteral(":id"), lI);
        lQuery.bindValue(QStringLiteral(":channel"), randomFrom(sChannels));
        lQuery.bindValue(QStringLiteral(":operation"), randomFrom(sOperations));
        lQuery.bindValue(QStringLiteral(":address"), 0x1000 + (lI % 8192) * 4);
        lQuery.bindValue(QStringLiteral(":burst_length"), (lI % 256) + 1);
        lQuery.bindValue(QStringLiteral(":response_code"), randomFrom(sResponses));
        lQuery.bindValue(QStringLiteral(":cycle"), 500 + lI * 2);
        lQuery.bindValue(QStringLiteral(":status"), randomFrom(sStatuses));
        if (!lQuery.exec()) {
            lDb.rollback();
            return false;
        }
    }

    return lDb.commit();
}

int DatabaseManager::mTableCount(const QString& xTableName) const
{
    QSqlQuery lQuery(mDatabase());
    lQuery.exec(QStringLiteral("SELECT COUNT(*) FROM %1").arg(xTableName));
    return lQuery.next() ? lQuery.value(0).toInt() : 0;
}

QSqlDatabase DatabaseManager::mDatabase() const
{
    return QSqlDatabase::database(dConnectionName);
}
