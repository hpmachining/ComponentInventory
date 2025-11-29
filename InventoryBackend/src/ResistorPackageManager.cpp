#include "ResistorPackageManager.h"
#include "DbUtils.h"
#include <sqlite3.h>

// Add a new package
bool ResistorPackageManager::addPackage(const ResistorPackage& pkg, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("INSERT INTO ResistorPackage (Name) VALUES (?);", stmt, result))
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

// Get package by ID
bool ResistorPackageManager::getPackageById(int id, ResistorPackage& pkg, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name FROM ResistorPackage WHERE ID=?;", stmt, result))
        return false;

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        pkg.id = sqlite3_column_int(stmt, 0);
        pkg.name = safeColumnText(stmt, 1);
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

// Update package
bool ResistorPackageManager::updatePackage(const ResistorPackage& pkg, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("UPDATE ResistorPackage SET Name=? WHERE ID=?;", stmt, result))
        return false;

    sqlite3_bind_text(stmt, 1, pkg.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, pkg.id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Delete package
bool ResistorPackageManager::deletePackage(int id, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("DELETE FROM ResistorPackage WHERE ID=?;", stmt, result))
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

// List all packages
bool ResistorPackageManager::listPackages(std::vector<ResistorPackage>& pkgs, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name FROM ResistorPackage;", stmt, result))
        return false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ResistorPackage pkg;
        pkg.id = sqlite3_column_int(stmt, 0);
        pkg.name = safeColumnText(stmt, 1);
        pkgs.push_back(pkg);
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}
