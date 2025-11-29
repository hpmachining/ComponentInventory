#include "ResistorManager.h"
#include "DbUtils.h"
#include <sqlite3.h>

// Add a new resistor
bool ResistorManager::addResistor(const Resistor& r, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("INSERT INTO Resistors "
        "(ComponentID, Resistance, Tolerance, PowerRating, TempCoefficient, "
        "PackageTypeID, CompositionID, LeadSpacing, VoltageRating) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);", stmt, result))
        return false;

    sqlite3_bind_int(stmt, 1, r.componentId);
    sqlite3_bind_double(stmt, 2, r.resistance);
    sqlite3_bind_double(stmt, 3, r.tolerance);
    sqlite3_bind_double(stmt, 4, r.powerRating);
    sqlite3_bind_double(stmt, 5, r.tempCoefficient);
    sqlite3_bind_int(stmt, 6, r.packageTypeId);
    sqlite3_bind_int(stmt, 7, r.compositionId);
    sqlite3_bind_double(stmt, 8, r.leadSpacing);
    sqlite3_bind_double(stmt, 9, r.voltageRating);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Get resistor by ComponentID
bool ResistorManager::getResistorByComponentId(int compId, Resistor& r, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ComponentID, Resistance, Tolerance, PowerRating, TempCoefficient, "
        "PackageTypeID, CompositionID, LeadSpacing, VoltageRating "
        "FROM Resistors WHERE ComponentID=?;", stmt, result))
        return false;

    sqlite3_bind_int(stmt, 1, compId);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        r.componentId = sqlite3_column_int(stmt, 0);
        r.resistance = sqlite3_column_double(stmt, 1);
        r.tolerance = sqlite3_column_double(stmt, 2);
        r.powerRating = sqlite3_column_double(stmt, 3);
        r.tempCoefficient = sqlite3_column_double(stmt, 4);
        r.packageTypeId = sqlite3_column_int(stmt, 5);
        r.compositionId = sqlite3_column_int(stmt, 6);
        r.leadSpacing = sqlite3_column_double(stmt, 7);
        r.voltageRating = sqlite3_column_double(stmt, 8);
    }
    else {
        result.setError(sqlite3_errcode(db_.handle()), "Resistor not found");
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Update resistor
bool ResistorManager::updateResistor(const Resistor& r, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("UPDATE Resistors SET Resistance=?, Tolerance=?, PowerRating=?, TempCoefficient=?, "
        "PackageTypeID=?, CompositionID=?, LeadSpacing=?, VoltageRating=? "
        "WHERE ComponentID=?;", stmt, result))
        return false;

    sqlite3_bind_double(stmt, 1, r.resistance);
    sqlite3_bind_double(stmt, 2, r.tolerance);
    sqlite3_bind_double(stmt, 3, r.powerRating);
    sqlite3_bind_double(stmt, 4, r.tempCoefficient);
    sqlite3_bind_int(stmt, 5, r.packageTypeId);
    sqlite3_bind_int(stmt, 6, r.compositionId);
    sqlite3_bind_double(stmt, 7, r.leadSpacing);
    sqlite3_bind_double(stmt, 8, r.voltageRating);
    sqlite3_bind_int(stmt, 9, r.componentId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Delete resistor
bool ResistorManager::deleteResistor(int compId, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("DELETE FROM Resistors WHERE ComponentID=?;", stmt, result))
        return false;

    sqlite3_bind_int(stmt, 1, compId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// List all resistors
bool ResistorManager::listResistors(std::vector<Resistor>& resistors, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ComponentID, Resistance, Tolerance, PowerRating, TempCoefficient, "
        "PackageTypeID, CompositionID, LeadSpacing, VoltageRating FROM Resistors;", stmt, result))
        return false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Resistor r;
        r.componentId = sqlite3_column_int(stmt, 0);
        r.resistance = sqlite3_column_double(stmt, 1);
        r.tolerance = sqlite3_column_double(stmt, 2);
        r.powerRating = sqlite3_column_double(stmt, 3);
        r.tempCoefficient = sqlite3_column_double(stmt, 4);
        r.packageTypeId = sqlite3_column_int(stmt, 5);
        r.compositionId = sqlite3_column_int(stmt, 6);
        r.leadSpacing = sqlite3_column_double(stmt, 7);
        r.voltageRating = sqlite3_column_double(stmt, 8);
        resistors.push_back(r);
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}
