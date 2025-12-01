#include "FuseManager.h"
#include <sqlite3.h>

FuseManager::FuseManager(Database& db) : db_(db) {}

bool FuseManager::addFuse(const Fuse& fuse, DbResult& res) {
    const char* sql = "INSERT INTO Fuses (ComponentId, PackageId, TypeId) VALUES (?,?,?);";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;

    sqlite3_bind_int(stmt, 1, fuse.componentId);
    sqlite3_bind_int(stmt, 2, fuse.packageId);
    sqlite3_bind_int(stmt, 3, fuse.typeId);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok) res.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
    sqlite3_finalize(stmt);
    return ok;
}

bool FuseManager::getFuseByComponentId(int componentId, Fuse& fuse, DbResult& res) {
    const char* sql = "SELECT ComponentId, PackageId, TypeId FROM Fuses WHERE ComponentId=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;

    sqlite3_bind_int(stmt, 1, componentId);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        fuse.componentId = sqlite3_column_int(stmt, 0);
        fuse.packageId = sqlite3_column_int(stmt, 1);
        fuse.typeId = sqlite3_column_int(stmt, 2);
        sqlite3_finalize(stmt);
        return true;
    }
    sqlite3_finalize(stmt);
    return false;
}

bool FuseManager::updateFuse(const Fuse& fuse, DbResult& res) {
    const char* sql = "UPDATE Fuses SET PackageId=?, TypeId=? WHERE ComponentId=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;

    sqlite3_bind_int(stmt, 1, fuse.packageId);
    sqlite3_bind_int(stmt, 2, fuse.typeId);
    sqlite3_bind_int(stmt, 3, fuse.componentId);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok) res.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
    sqlite3_finalize(stmt);
    return ok;
}

bool FuseManager::deleteFuse(int componentId, DbResult& res) {
    const char* sql = "DELETE FROM Fuses WHERE ComponentId=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;

    sqlite3_bind_int(stmt, 1, componentId);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok) res.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
    sqlite3_finalize(stmt);
    return ok;
}

bool FuseManager::listFuses(std::vector<Fuse>& fuses, DbResult& res) {
    const char* sql = "SELECT ComponentId, PackageId, TypeId FROM Fuses;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Fuse f;
        f.componentId = sqlite3_column_int(stmt, 0);
        f.packageId = sqlite3_column_int(stmt, 1);
        f.typeId = sqlite3_column_int(stmt, 2);
        fuses.push_back(f);
    }
    sqlite3_finalize(stmt);
    return true;
}

int FuseManager::getPackageByName(const std::string& name, DbResult& res) {
    return resolveIdByName("FusePackage", name, res);
}

int FuseManager::getTypeByName(const std::string& name, DbResult& res) {
    return resolveIdByName("FuseType", name, res);
}

int FuseManager::resolveIdByName(const std::string& table, const std::string& name, DbResult& res) {
    std::string sql = "SELECT Id FROM " + table + " WHERE Name=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql.c_str(), stmt, res)) return -1;

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    int id = (rc == SQLITE_ROW) ? sqlite3_column_int(stmt, 0) : -1;
    sqlite3_finalize(stmt);
    return id;
}
