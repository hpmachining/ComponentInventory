#include "DiodeManager.h"
#include <sqlite3.h>

DiodeManager::DiodeManager(Database& db)
    : db_(db) {
}

// --- CRUD ---

bool DiodeManager::add(const Diode& d, DbResult& res) {
    const char* sql = "INSERT INTO Diodes "
        "(ComponentId, PackageId, TypeId, PolarityId, ForwardVoltage, MaxCurrent, MaxReverseVoltage, ReverseLeakage) "
        "VALUES (?,?,?,?,?,?,?,?);";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;

    sqlite3_bind_int(stmt, 1, d.componentId);
    sqlite3_bind_int(stmt, 2, d.packageId);
    sqlite3_bind_int(stmt, 3, d.typeId);
    sqlite3_bind_int(stmt, 4, d.polarityId);
    sqlite3_bind_double(stmt, 5, d.forwardVoltage);
    sqlite3_bind_double(stmt, 6, d.maxCurrent);
    sqlite3_bind_double(stmt, 7, d.maxReverseVoltage);
    sqlite3_bind_double(stmt, 8, d.reverseLeakage);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok) res.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
    sqlite3_finalize(stmt);
    return ok;
}

bool DiodeManager::getById(int componentId, Diode& d, DbResult& res) {
    const char* sql = "SELECT ComponentId, PackageId, TypeId, PolarityId, ForwardVoltage, MaxCurrent, MaxReverseVoltage, ReverseLeakage "
        "FROM Diodes WHERE ComponentId=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;

    sqlite3_bind_int(stmt, 1, componentId);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        d.componentId = sqlite3_column_int(stmt, 0);
        d.packageId = sqlite3_column_int(stmt, 1);
        d.typeId = sqlite3_column_int(stmt, 2);
        d.polarityId = sqlite3_column_int(stmt, 3);
        d.forwardVoltage = sqlite3_column_double(stmt, 4);
        d.maxCurrent = sqlite3_column_double(stmt, 5);
        d.maxReverseVoltage = sqlite3_column_double(stmt, 6);
        d.reverseLeakage = sqlite3_column_double(stmt, 7);
        sqlite3_finalize(stmt);
        return true;
    }

    sqlite3_finalize(stmt);
    return false;
}

bool DiodeManager::update(const Diode& d, DbResult& res) {
    const char* sql = "UPDATE Diodes SET "
        "PackageId=?, TypeId=?, PolarityId=?, ForwardVoltage=?, MaxCurrent=?, MaxReverseVoltage=?, ReverseLeakage=? "
        "WHERE ComponentId=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;

    sqlite3_bind_int(stmt, 1, d.packageId);
    sqlite3_bind_int(stmt, 2, d.typeId);
    sqlite3_bind_int(stmt, 3, d.polarityId);
    sqlite3_bind_double(stmt, 4, d.forwardVoltage);
    sqlite3_bind_double(stmt, 5, d.maxCurrent);
    sqlite3_bind_double(stmt, 6, d.maxReverseVoltage);
    sqlite3_bind_double(stmt, 7, d.reverseLeakage);
    sqlite3_bind_int(stmt, 8, d.componentId);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok) res.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
    sqlite3_finalize(stmt);
    return ok;
}

bool DiodeManager::remove(int componentId, DbResult& res) {
    const char* sql = "DELETE FROM Diodes WHERE ComponentId=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;

    sqlite3_bind_int(stmt, 1, componentId);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok) res.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
    sqlite3_finalize(stmt);
    return ok;
}

bool DiodeManager::list(std::vector<Diode>& ds, DbResult& res) {
    const char* sql = "SELECT ComponentId, PackageId, TypeId, PolarityId, ForwardVoltage, MaxCurrent, MaxReverseVoltage, ReverseLeakage "
        "FROM Diodes;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Diode d;
        d.componentId = sqlite3_column_int(stmt, 0);
        d.packageId = sqlite3_column_int(stmt, 1);
        d.typeId = sqlite3_column_int(stmt, 2);
        d.polarityId = sqlite3_column_int(stmt, 3);
        d.forwardVoltage = sqlite3_column_double(stmt, 4);
        d.maxCurrent = sqlite3_column_double(stmt, 5);
        d.maxReverseVoltage = sqlite3_column_double(stmt, 6);
        d.reverseLeakage = sqlite3_column_double(stmt, 7);
        ds.push_back(d);
    }

    sqlite3_finalize(stmt);
    return true;
}
