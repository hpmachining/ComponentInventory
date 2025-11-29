#include "ElectrolyticManager.h"
#include "DbUtils.h"
#include <sqlite3.h>

ElectrolyticManager::ElectrolyticManager(Database& db) : db_(db) {}

// Add electrolytic subtype
bool ElectrolyticManager::addElectrolytic(const Electrolytic& ecap, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "INSERT INTO Electrolytics (ComponentID, Diameter, Height, LeadSpacing) "
        "VALUES (?, ?, ?, ?);",
        stmt, result)) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, ecap.componentId);
    sqlite3_bind_double(stmt, 2, ecap.diameter);
    sqlite3_bind_double(stmt, 3, ecap.height);
    sqlite3_bind_double(stmt, 4, ecap.leadSpacing);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Get electrolytic by ID
bool ElectrolyticManager::getElectrolyticById(int id, Electrolytic& ecap, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "SELECT ComponentID, Diameter, Height, LeadSpacing "
        "FROM Electrolytics WHERE ComponentID=?;",
        stmt, result)) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        ecap.componentId = sqlite3_column_int(stmt, 0);
        ecap.diameter = sqlite3_column_double(stmt, 1);
        ecap.height = sqlite3_column_double(stmt, 2);
        ecap.leadSpacing = sqlite3_column_double(stmt, 3);
    }
    else {
        result.setError(sqlite3_errcode(db_.handle()), "Electrolytic not found");
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Update electrolytic
bool ElectrolyticManager::updateElectrolytic(const Electrolytic& ecap, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "UPDATE Electrolytics SET Diameter=?, Height=?, LeadSpacing=? "
        "WHERE ComponentID=?;",
        stmt, result)) {
        return false;
    }

    sqlite3_bind_double(stmt, 1, ecap.diameter);
    sqlite3_bind_double(stmt, 2, ecap.height);
    sqlite3_bind_double(stmt, 3, ecap.leadSpacing);
    sqlite3_bind_int(stmt, 4, ecap.componentId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Delete electrolytic
bool ElectrolyticManager::deleteElectrolytic(int id, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("DELETE FROM Electrolytics WHERE ComponentID=?;", stmt, result)) {
        return false;
    }

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

// List all electrolytics
bool ElectrolyticManager::listElectrolytics(std::vector<Electrolytic>& ecaps, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "SELECT ComponentID, Diameter, Height, LeadSpacing FROM Electrolytics;",
        stmt, result)) {
        return false;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Electrolytic ecap;
        ecap.componentId = sqlite3_column_int(stmt, 0);
        ecap.diameter = sqlite3_column_double(stmt, 1);
        ecap.height = sqlite3_column_double(stmt, 2);
        ecap.leadSpacing = sqlite3_column_double(stmt, 3);
        ecaps.push_back(ecap);
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}
