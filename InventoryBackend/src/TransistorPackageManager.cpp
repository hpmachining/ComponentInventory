#include "TransistorPackageManager.h"

TransistorPackageManager::TransistorPackageManager(Database& db) : db_(db) {}

bool TransistorPackageManager::add(const TransistorPackage& pkg, DbResult& result) {
    return db_.exec("INSERT INTO TransistorPackage (Name) VALUES ('" + pkg.name + "');", result);
}

bool TransistorPackageManager::getById(int id, TransistorPackage& pkg, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name FROM TransistorPackage WHERE ID=?;", stmt, result)) return false;
    sqlite3_bind_int(stmt, 1, id);

    bool ok = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        pkg.id = sqlite3_column_int(stmt, 0);
        pkg.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        ok = true;
    }
    db_.finalize(stmt);
    return ok;
}

bool TransistorPackageManager::list(std::vector<TransistorPackage>& pkgs, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name FROM TransistorPackage;", stmt, result)) return false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        TransistorPackage p;
        p.id = sqlite3_column_int(stmt, 0);
        p.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        pkgs.push_back(p);
    }
    db_.finalize(stmt);
    return true;
}

bool TransistorPackageManager::remove(int id, DbResult& result) {
    return db_.exec("DELETE FROM TransistorPackage WHERE ID=" + std::to_string(id) + ";", result);
}

int TransistorPackageManager::getByName(const std::string& name, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID FROM TransistorPackage WHERE Name=?;", stmt, result)) return -1;
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    int id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return id;
}
