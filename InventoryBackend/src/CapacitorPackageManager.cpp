#include "CapacitorPackageManager.h"
#include <sqlite3.h>

CapacitorPackageManager::CapacitorPackageManager(Database& db) : db_(db) {}

bool CapacitorPackageManager::addPackage(const CapacitorPackage& pkg, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("INSERT INTO CapacitorPackage (Name) VALUES (?);", stmt, result))
        return false;

    sqlite3_bind_text(stmt, 1, pkg.name.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }
    db_.finalize(stmt);
    result.clear();
    return true;
}

bool CapacitorPackageManager::getPackageById(int id, CapacitorPackage& pkg, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name FROM CapacitorPackage WHERE ID=?;", stmt, result))
        return false;

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        pkg.id = sqlite3_column_int(stmt, 0);
        pkg.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    }
    else {
        result.setError(sqlite3_errcode(db_.handle()), "Package not found");
        db_.finalize(stmt);
        return false;
    }
    db_.finalize(stmt);
    result.clear();
    return true;
}

bool CapacitorPackageManager::listPackages(std::vector<CapacitorPackage>& pkgs, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name FROM CapacitorPackage;", stmt, result))
        return false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        CapacitorPackage pkg;
        pkg.id = sqlite3_column_int(stmt, 0);
        pkg.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        pkgs.push_back(pkg);
    }
    db_.finalize(stmt);
    result.clear();
    return true;
}

bool CapacitorPackageManager::deletePackage(int id, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("DELETE FROM CapacitorPackage WHERE ID=?;", stmt, result))
        return false;

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }
    db_.finalize(stmt);
    result.clear();
    return true;
}
