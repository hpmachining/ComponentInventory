#include "ResistorManager.h"
#include "DbUtils.h"
#include <sqlite3.h>

// Add resistor
bool ResistorManager::add(const Resistor& r, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "INSERT INTO Resistors ("
        "ComponentID, Resistance, Tolerance, PowerRating, "
        "TempCoeffMin, TempCoeffMax, "
        "TempMin, TempMax, "
        "PackageTypeID, CompositionID, LeadSpacing, VoltageRating"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    if (!db_.prepare(sql, stmt, result))
        return false;

    int idx = 1;
    sqlite3_bind_int(stmt, idx++, r.componentId);
    sqlite3_bind_double(stmt, idx++, r.resistance);
    sqlite3_bind_double(stmt, idx++, r.tolerance);
    sqlite3_bind_double(stmt, idx++, r.powerRating);

    // TCR
    if (r.hasTempCoeff) {
        sqlite3_bind_double(stmt, idx++, r.tempCoeffMin);
        sqlite3_bind_double(stmt, idx++, r.tempCoeffMax);
    }
    else {
        sqlite3_bind_null(stmt, idx++);
        sqlite3_bind_null(stmt, idx++);
    }

    // Temperature range
    if (r.hasTempRange) {
        sqlite3_bind_double(stmt, idx++, r.tempMin);
        sqlite3_bind_double(stmt, idx++, r.tempMax);
    }
    else {
        sqlite3_bind_null(stmt, idx++);
        sqlite3_bind_null(stmt, idx++);
    }

    sqlite3_bind_int(stmt, idx++, r.packageTypeId);
    sqlite3_bind_int(stmt, idx++, r.compositionId);
    sqlite3_bind_double(stmt, idx++, r.leadSpacing);
    sqlite3_bind_double(stmt, idx++, r.voltageRating);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Get resistor by component ID
bool ResistorManager::getByComponentId(int compId, Resistor& r, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT "
        "ComponentID, Resistance, Tolerance, PowerRating, "
        "TempCoeffMin, TempCoeffMax, "
        "TempMin, TempMax, "
        "PackageTypeID, CompositionID, LeadSpacing, VoltageRating "
        "FROM Resistors WHERE ComponentID=?;";

    if (!db_.prepare(sql, stmt, result))
        return false;

    sqlite3_bind_int(stmt, 1, compId);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int col = 0;
        r.componentId = sqlite3_column_int(stmt, col++);
        r.resistance = sqlite3_column_double(stmt, col++);
        r.tolerance = sqlite3_column_double(stmt, col++);
        r.powerRating = sqlite3_column_double(stmt, col++);

        // TCR
        if (sqlite3_column_type(stmt, col) == SQLITE_NULL) {
            r.hasTempCoeff = false;
            col += 2;
        }
        else {
            r.tempCoeffMin = sqlite3_column_double(stmt, col++);
            r.tempCoeffMax = sqlite3_column_double(stmt, col++);
            r.hasTempCoeff = true;
        }

        // Temperature range
        if (sqlite3_column_type(stmt, col) == SQLITE_NULL) {
            r.hasTempRange = false;
            col += 2;
        }
        else {
            r.tempMin = sqlite3_column_double(stmt, col++);
            r.tempMax = sqlite3_column_double(stmt, col++);
            r.hasTempRange = true;
        }

        r.packageTypeId = sqlite3_column_int(stmt, col++);
        r.compositionId = sqlite3_column_int(stmt, col++);
        r.leadSpacing = sqlite3_column_double(stmt, col++);
        r.voltageRating = sqlite3_column_double(stmt, col++);
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
bool ResistorManager::update(const Resistor& r, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "UPDATE Resistors SET "
        "Resistance=?, Tolerance=?, PowerRating=?, "
        "TempCoeffMin=?, TempCoeffMax=?, "
        "TempMin=?, TempMax=?, "
        "PackageTypeID=?, CompositionID=?, LeadSpacing=?, VoltageRating=? "
        "WHERE ComponentID=?;";

    if (!db_.prepare(sql, stmt, result))
        return false;

    int idx = 1;
    sqlite3_bind_double(stmt, idx++, r.resistance);
    sqlite3_bind_double(stmt, idx++, r.tolerance);
    sqlite3_bind_double(stmt, idx++, r.powerRating);

    // TCR
    if (r.hasTempCoeff) {
        sqlite3_bind_double(stmt, idx++, r.tempCoeffMin);
        sqlite3_bind_double(stmt, idx++, r.tempCoeffMax);
    }
    else {
        sqlite3_bind_null(stmt, idx++);
        sqlite3_bind_null(stmt, idx++);
    }

    // Temperature range
    if (r.hasTempRange) {
        sqlite3_bind_double(stmt, idx++, r.tempMin);
        sqlite3_bind_double(stmt, idx++, r.tempMax);
    }
    else {
        sqlite3_bind_null(stmt, idx++);
        sqlite3_bind_null(stmt, idx++);
    }

    sqlite3_bind_int(stmt, idx++, r.packageTypeId);
    sqlite3_bind_int(stmt, idx++, r.compositionId);
    sqlite3_bind_double(stmt, idx++, r.leadSpacing);
    sqlite3_bind_double(stmt, idx++, r.voltageRating);
    sqlite3_bind_int(stmt, idx++, r.componentId);

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
bool ResistorManager::remove(int compId, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "DELETE FROM Resistors WHERE ComponentID=?;";

    if (!db_.prepare(sql, stmt, result))
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
bool ResistorManager::list(std::vector<Resistor>& resistors, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql =
        "SELECT "
        "ComponentID, Resistance, Tolerance, PowerRating, "
        "TempCoeffMin, TempCoeffMax, "
        "TempMin, TempMax, "
        "PackageTypeID, CompositionID, LeadSpacing, VoltageRating "
        "FROM Resistors;";

    if (!db_.prepare(sql, stmt, result))
        return false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Resistor r;
        int col = 0;

        r.componentId = sqlite3_column_int(stmt, col++);
        r.resistance = sqlite3_column_double(stmt, col++);
        r.tolerance = sqlite3_column_double(stmt, col++);
        r.powerRating = sqlite3_column_double(stmt, col++);

        // TCR
        if (sqlite3_column_type(stmt, col) == SQLITE_NULL) {
            r.hasTempCoeff = false;
            col += 2;
        }
        else {
            r.tempCoeffMin = sqlite3_column_double(stmt, col++);
            r.tempCoeffMax = sqlite3_column_double(stmt, col++);
            r.hasTempCoeff = true;
        }

        // Temperature range
        if (sqlite3_column_type(stmt, col) == SQLITE_NULL) {
            r.hasTempRange = false;
            col += 2;
        }
        else {
            r.tempMin = sqlite3_column_double(stmt, col++);
            r.tempMax = sqlite3_column_double(stmt, col++);
            r.hasTempRange = true;
        }

        r.packageTypeId = sqlite3_column_int(stmt, col++);
        r.compositionId = sqlite3_column_int(stmt, col++);
        r.leadSpacing = sqlite3_column_double(stmt, col++);
        r.voltageRating = sqlite3_column_double(stmt, col++);

        resistors.push_back(r);
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}
