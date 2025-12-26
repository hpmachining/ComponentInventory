#include "FusePackageManager.h"
#include <sqlite3.h>

FusePackageManager::FusePackageManager(Database& db) : db_(db) {}

bool FusePackageManager::add(const FusePackage& pkg, DbResult& res) {
    const char* sql = "INSERT INTO FusePackage (Name) VALUES (?);";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;
    sqlite3_bind_text(stmt, 1, pkg.name.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok) res.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
    sqlite3_finalize(stmt);
    return ok;
}

bool FusePackageManager::getById(int id, FusePackage& pkg, DbResult& res) {
    const char* sql = "SELECT Id, Name FROM FusePackage WHERE Id=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        pkg.id = sqlite3_column_int(stmt, 0);
        pkg.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        sqlite3_finalize(stmt);
        return true;
    }
    sqlite3_finalize(stmt);
    return false;
}

int FusePackageManager::getByName(const std::string& name, DbResult& res) {
    const char* sql = "SELECT Id FROM FusePackage WHERE Name=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return -1;
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    int id = (rc == SQLITE_ROW) ? sqlite3_column_int(stmt, 0) : -1;
    sqlite3_finalize(stmt);
    return id;
}

bool FusePackageManager::list(std::vector<FusePackage>& pkgs, DbResult& res) {
    const char* sql = "SELECT Id, Name FROM FusePackage;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        FusePackage pkg;
        pkg.id = sqlite3_column_int(stmt, 0);
        pkg.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        pkgs.push_back(pkg);
    }
    sqlite3_finalize(stmt);
    return true;
}

bool FusePackageManager::remove(int id, DbResult& res) {
    const char* sql = "DELETE FROM FusePackage WHERE Id=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;
    sqlite3_bind_int(stmt, 1, id);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok) res.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
    sqlite3_finalize(stmt);
    return ok;
}
