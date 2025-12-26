#include "CapacitorDielectricManager.h"
#include <sqlite3.h>

CapacitorDielectricManager::CapacitorDielectricManager(Database& db) : db_(db) {}

bool CapacitorDielectricManager::add(const CapacitorDielectric& diel, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("INSERT INTO CapacitorDielectric (Name) VALUES (?);", stmt, result))
        return false;

    sqlite3_bind_text(stmt, 1, diel.name.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

bool CapacitorDielectricManager::getById(int id, CapacitorDielectric& diel, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name FROM CapacitorDielectric WHERE ID=?;", stmt, result))
        return false;

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        diel.id = sqlite3_column_int(stmt, 0);
        diel.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        db_.finalize(stmt);
        result.clear();
        return true;
    }
    db_.finalize(stmt);
    result.setError(0, "Dielectric not found");
    return false;
}

bool CapacitorDielectricManager::list(std::vector<CapacitorDielectric>& diels, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name FROM CapacitorDielectric;", stmt, result))
        return false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        CapacitorDielectric diel;
        diel.id = sqlite3_column_int(stmt, 0);
        diel.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        diels.push_back(diel);
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

bool CapacitorDielectricManager::remove(int id, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("DELETE FROM CapacitorDielectric WHERE ID=?;", stmt, result))
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

int CapacitorDielectricManager::getByName(const std::string& name, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID FROM CapacitorDielectric WHERE Name=?;", stmt, result))
        return -1;

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    int id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        id = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    return id;
}
