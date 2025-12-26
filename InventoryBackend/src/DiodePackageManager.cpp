#include "DiodePackageManager.h"
#include <sqlite3.h>

DiodePackageManager::DiodePackageManager(Database& db)
    : db_(db) {
}

bool DiodePackageManager::add(const DiodePackage& pkg, DbResult& res) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "INSERT INTO DiodePackage (Name) VALUES (?);",
        stmt, res))
        return false;

    sqlite3_bind_text(stmt, 1, pkg.name.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        res.setError(sqlite3_errcode(db_.handle()),
            sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    res.clear();
    return true;
}

bool DiodePackageManager::getById(int id, DiodePackage& pkg, DbResult& res) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "SELECT ID, Name FROM DiodePackage WHERE ID=?;",
        stmt, res))
        return false;

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        pkg.id = sqlite3_column_int(stmt, 0);
        pkg.name = reinterpret_cast<const char*>(
            sqlite3_column_text(stmt, 1));
    }
    else {
        res.setError(sqlite3_errcode(db_.handle()),
            "Diode package not found");
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    res.clear();
    return true;
}

int DiodePackageManager::getByName(const std::string& name, DbResult& res) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "SELECT ID FROM DiodePackage WHERE Name=?;",
        stmt, res))
        return -1;

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    int id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        id = sqlite3_column_int(stmt, 0);

    db_.finalize(stmt);
    return id;
}

bool DiodePackageManager::list(std::vector<DiodePackage>& pkgs, DbResult& res) {
    pkgs.clear();

    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "SELECT ID, Name FROM DiodePackage ORDER BY Name;",
        stmt, res))
        return false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        DiodePackage pkg;
        pkg.id = sqlite3_column_int(stmt, 0);
        pkg.name = reinterpret_cast<const char*>(
            sqlite3_column_text(stmt, 1));
        pkgs.push_back(pkg);
    }

    db_.finalize(stmt);
    res.clear();
    return true;
}

bool DiodePackageManager::remove(int id, DbResult& res) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "DELETE FROM DiodePackage WHERE ID=?;",
        stmt, res))
        return false;

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        res.setError(sqlite3_errcode(db_.handle()),
            sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    res.clear();
    return true;
}
