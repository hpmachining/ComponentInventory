#include "CapacitorManager.h"
#include "DbUtils.h"
#include <sqlite3.h>

CapacitorManager::CapacitorManager(Database& db) : db_(db) {}

// Add capacitor
bool CapacitorManager::add(const Capacitor& cap, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "INSERT INTO Capacitors "
        "(ComponentID, Capacitance, VoltageRating, Tolerance, ESR, LeakageCurrent, "
        "Polarized, PackageTypeID, DielectricTypeID, "
        "Diameter, Height, LeadSpacing, Length, Width) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);",
        stmt, result))
    {
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
    sqlite3_bind_double(stmt, 10, cap.diameter);
    sqlite3_bind_double(stmt, 11, cap.height);
    sqlite3_bind_double(stmt, 12, cap.leadSpacing);
    sqlite3_bind_double(stmt, 13, cap.length);
    sqlite3_bind_double(stmt, 14, cap.width);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Get capacitor by component ID
bool CapacitorManager::getById(int id, Capacitor& cap, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "SELECT ComponentID, Capacitance, VoltageRating, Tolerance, ESR, LeakageCurrent, "
        "Polarized, PackageTypeID, DielectricTypeID, "
        "Diameter, Height, LeadSpacing, Length, Width "
        "FROM Capacitors WHERE ComponentID=?;",
        stmt, result))
    {
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

        cap.diameter = sqlite3_column_double(stmt, 9);
        cap.height = sqlite3_column_double(stmt, 10);
        cap.leadSpacing = sqlite3_column_double(stmt, 11);
        cap.length = sqlite3_column_double(stmt, 12);
        cap.width = sqlite3_column_double(stmt, 13);
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

// Update capacitor
bool CapacitorManager::update(const Capacitor& cap, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "UPDATE Capacitors SET "
        "Capacitance=?, VoltageRating=?, Tolerance=?, ESR=?, LeakageCurrent=?, "
        "Polarized=?, PackageTypeID=?, DielectricTypeID=?, "
        "Diameter=?, Height=?, LeadSpacing=?, Length=?, Width=? "
        "WHERE ComponentID=?;",
        stmt, result))
    {
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
    sqlite3_bind_double(stmt, 9, cap.diameter);
    sqlite3_bind_double(stmt, 10, cap.height);
    sqlite3_bind_double(stmt, 11, cap.leadSpacing);
    sqlite3_bind_double(stmt, 12, cap.length);
    sqlite3_bind_double(stmt, 13, cap.width);
    sqlite3_bind_int(stmt, 14, cap.componentId);

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
bool CapacitorManager::remove(int id, DbResult& result) {
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
bool CapacitorManager::list(std::vector<Capacitor>& caps, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "SELECT ComponentID, Capacitance, VoltageRating, Tolerance, ESR, LeakageCurrent, "
        "Polarized, PackageTypeID, DielectricTypeID, "
        "Diameter, Height, LeadSpacing, Length, Width "
        "FROM Capacitors;",
        stmt, result))
    {
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

        cap.diameter = sqlite3_column_double(stmt, 9);
        cap.height = sqlite3_column_double(stmt, 10);
        cap.leadSpacing = sqlite3_column_double(stmt, 11);
        cap.length = sqlite3_column_double(stmt, 12);
        cap.width = sqlite3_column_double(stmt, 13);

        caps.push_back(std::move(cap));
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}
