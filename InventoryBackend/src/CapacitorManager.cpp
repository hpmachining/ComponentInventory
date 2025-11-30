#include "CapacitorManager.h"
#include "DbUtils.h"   // for safeColumnText
#include <sqlite3.h>

CapacitorManager::CapacitorManager(Database& db) : db_(db) {}

// Add a new capacitor
bool CapacitorManager::addCapacitor(const Capacitor& cap, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "INSERT INTO Capacitors (ComponentID, Capacitance, VoltageRating, "
        "Tolerance, ESR, LeakageCurrent, Polarized, PackageTypeID, DielectricTypeID) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);",
        stmt, result)) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, cap.componentId);
    sqlite3_bind_double(stmt, 2, cap.capacitance);
    sqlite3_bind_double(stmt, 3, cap.voltageRating);
    sqlite3_bind_double(stmt, 4, cap.tolerance);
    sqlite3_bind_double(stmt, 5, cap.esr);
    sqlite3_bind_double(stmt, 6, cap.leakageCurrent);
    sqlite3_bind_int(stmt, 7, cap.polarized ? 1 : 0);
    sqlite3_bind_int(stmt, 8, cap.packageTypeId);
    sqlite3_bind_int(stmt, 9, cap.dielectricTypeId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Get capacitor by ID
bool CapacitorManager::getCapacitorById(int id, Capacitor& cap, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "SELECT ComponentID, Capacitance, VoltageRating, Tolerance, ESR, "
        "LeakageCurrent, Polarized, PackageTypeID, DielectricTypeID "
        "FROM Capacitors WHERE ComponentID=?;",
        stmt, result)) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        cap.componentId = sqlite3_column_int(stmt, 0);
        cap.capacitance = sqlite3_column_double(stmt, 1);
        cap.voltageRating = sqlite3_column_double(stmt, 2);
        cap.tolerance = sqlite3_column_double(stmt, 3);
        cap.esr = sqlite3_column_double(stmt, 4);
        cap.leakageCurrent = sqlite3_column_double(stmt, 5);
        cap.polarized = sqlite3_column_int(stmt, 6) != 0;
        cap.packageTypeId = sqlite3_column_int(stmt, 7);
        cap.dielectricTypeId = sqlite3_column_int(stmt, 8);
    }
    else {
        result.setError(sqlite3_errcode(db_.handle()), "Capacitor not found");
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

bool CapacitorManager::getCapacitorByComponentId(int componentId, Capacitor& cap, DbResult& result) {
    return getCapacitorById(componentId, cap, result);
}

// Update capacitor
bool CapacitorManager::updateCapacitor(const Capacitor& cap, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "UPDATE Capacitors SET Capacitance=?, VoltageRating=?, Tolerance=?, ESR=?, "
        "LeakageCurrent=?, Polarized=?, PackageTypeID=?, DielectricTypeID=? "
        "WHERE ComponentID=?;",
        stmt, result)) {
        return false;
    }

    sqlite3_bind_double(stmt, 1, cap.capacitance);
    sqlite3_bind_double(stmt, 2, cap.voltageRating);
    sqlite3_bind_double(stmt, 3, cap.tolerance);
    sqlite3_bind_double(stmt, 4, cap.esr);
    sqlite3_bind_double(stmt, 5, cap.leakageCurrent);
    sqlite3_bind_int(stmt, 6, cap.polarized ? 1 : 0);
    sqlite3_bind_int(stmt, 7, cap.packageTypeId);
    sqlite3_bind_int(stmt, 8, cap.dielectricTypeId);
    sqlite3_bind_int(stmt, 9, cap.componentId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Delete capacitor
bool CapacitorManager::deleteCapacitor(int id, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("DELETE FROM Capacitors WHERE ComponentID=?;", stmt, result)) {
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

// List all capacitors
bool CapacitorManager::listCapacitors(std::vector<Capacitor>& caps, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "SELECT ComponentID, Capacitance, VoltageRating, Tolerance, ESR, "
        "LeakageCurrent, Polarized, PackageTypeID, DielectricTypeID FROM Capacitors;",
        stmt, result)) {
        return false;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Capacitor cap;
        cap.componentId = sqlite3_column_int(stmt, 0);
        cap.capacitance = sqlite3_column_double(stmt, 1);
        cap.voltageRating = sqlite3_column_double(stmt, 2);
        cap.tolerance = sqlite3_column_double(stmt, 3);
        cap.esr = sqlite3_column_double(stmt, 4);
        cap.leakageCurrent = sqlite3_column_double(stmt, 5);
        cap.polarized = sqlite3_column_int(stmt, 6) != 0;
        cap.packageTypeId = sqlite3_column_int(stmt, 7);
        cap.dielectricTypeId = sqlite3_column_int(stmt, 8);
        caps.push_back(cap);
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}
