#include "ManufacturerManager.h"
#include "DbUtils.h"
#include <sqlite3.h>

// Add a new manufacturer
bool ManufacturerManager::addManufacturer(const Manufacturer& man, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("INSERT INTO Manufacturers (Name, Country, Website, Notes) VALUES (?, ?, ?, ?);", stmt, result))
        return false;

    sqlite3_bind_text(stmt, 1, man.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, man.country.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, man.website.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, man.notes.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Get manufacturer by ID
bool ManufacturerManager::getManufacturerById(int id, Manufacturer& man, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name, Country, Website, Notes FROM Manufacturers WHERE ID=?;", stmt, result))
        return false;

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        man.id = sqlite3_column_int(stmt, 0);
        man.name = safeColumnText(stmt, 1);
        man.country = safeColumnText(stmt, 2);
        man.website = safeColumnText(stmt, 3);
        man.notes = safeColumnText(stmt, 4);
    }
    else {
        result.setError(sqlite3_errcode(db_.handle()), "Manufacturer not found");
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Update manufacturer
bool ManufacturerManager::updateManufacturer(const Manufacturer& man, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("UPDATE Manufacturers SET Name=?, Country=?, Website=?, Notes=? WHERE ID=?;", stmt, result))
        return false;

    sqlite3_bind_text(stmt, 1, man.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, man.country.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, man.website.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, man.notes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, man.id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Delete manufacturer
bool ManufacturerManager::deleteManufacturer(int id, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("DELETE FROM Manufacturers WHERE ID=?;", stmt, result))
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

// List all manufacturers
bool ManufacturerManager::listManufacturers(std::vector<Manufacturer>& mans, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name, Country, Website, Notes FROM Manufacturers;", stmt, result))
        return false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Manufacturer man;
        man.id = sqlite3_column_int(stmt, 0);
        man.name = safeColumnText(stmt, 1);
        man.country = safeColumnText(stmt, 2);
        man.website = safeColumnText(stmt, 3);
        man.notes = safeColumnText(stmt, 4);
        mans.push_back(man);
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

int ManufacturerManager::getByName(const std::string& name, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID FROM Manufacturers WHERE Name=?;", stmt, result)) return -1;
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    int id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return id;
}
