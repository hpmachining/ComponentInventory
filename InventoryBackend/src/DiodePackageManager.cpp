#include "DiodePackageManager.h"
#include <sqlite3.h>

DiodePackageManager::DiodePackageManager(Database& db) : db_(db) {}

bool DiodePackageManager::addPackage(const DiodePackage& pkg, DbResult& res) {
    const char* sql = "INSERT INTO DiodePackage (Name) VALUES (?);";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;
    sqlite3_bind_text(stmt, 1, pkg.name.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok) res.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
    sqlite3_finalize(stmt);
    return ok;
}

bool DiodePackageManager::getPackageById(int id, DiodePackage& pkg, DbResult& res) {
    const char* sql = "SELECT Id, Name FROM DiodePackage WHERE Id=?;";
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

int DiodePackageManager::getByName(const std::string& name, DbResult& res) {
    const char* sql = "SELECT Id FROM DiodePackage WHERE Name=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return -1;
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    int id = (rc == SQLITE_ROW) ? sqlite3_column_int(stmt, 0) : -1;
    sqlite3_finalize(stmt);
    return id;
}

bool DiodePackageManager::listPackages(std::vector<DiodePackage>& pkgs, DbResult& res) {
    const char* sql = "SELECT Id, Name FROM DiodePackage;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        DiodePackage pkg;
        pkg.id = sqlite3_column_int(stmt, 0);
        pkg.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        pkgs.push_back(pkg);
    }
    sqlite3_finalize(stmt);
    return true;
}

bool DiodePackageManager::deletePackage(int id, DbResult& res) {
    const char* sql = "DELETE FROM DiodePackage WHERE Id=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;
    sqlite3_bind_int(stmt, 1, id);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok) res.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
    sqlite3_finalize(stmt);
    return ok;
}
