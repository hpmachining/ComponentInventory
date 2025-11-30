#include "ComponentManager.h"
#include "DbUtils.h"
#include <sqlite3.h>

// Add a new component
bool ComponentManager::addComponent(const Component& comp, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "INSERT INTO Components (CategoryID, PartNumber, ManufacturerID, "
        "Description, Notes, Quantity, DatasheetLink, CreatedOn, ModifiedOn) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, datetime('now'), datetime('now'));",
        stmt, result)) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, comp.categoryId);
    sqlite3_bind_text(stmt, 2, comp.partNumber.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, comp.manufacturerId);
    sqlite3_bind_text(stmt, 4, comp.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, comp.notes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, comp.quantity);
    sqlite3_bind_text(stmt, 7, comp.datasheetLink.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Get a component by ID
bool ComponentManager::getComponentById(int id, Component& comp, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "SELECT ID, CategoryID, PartNumber, ManufacturerID, Description, Notes, "
        "Quantity, DatasheetLink, CreatedOn, ModifiedOn FROM Components WHERE ID = ?;",
        stmt, result)) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        comp.id = sqlite3_column_int(stmt, 0);
        comp.categoryId = sqlite3_column_int(stmt, 1);
        comp.partNumber = safeColumnText(stmt, 2);
        comp.manufacturerId = sqlite3_column_int(stmt, 3);
        comp.description = safeColumnText(stmt, 4);
        comp.notes = safeColumnText(stmt, 5);
        comp.quantity = sqlite3_column_int(stmt, 6);
        comp.datasheetLink = safeColumnText(stmt, 7);
        comp.createdOn = safeColumnText(stmt, 8);
        comp.modifiedOn = safeColumnText(stmt, 9);
    }
    else {
        result.setError(sqlite3_errcode(db_.handle()), "Component not found");
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Update an existing component
bool ComponentManager::updateComponent(const Component& comp, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "UPDATE Components SET CategoryID=?, PartNumber=?, ManufacturerID=?, "
        "Description=?, Notes=?, Quantity=?, DatasheetLink=?, ModifiedOn=datetime('now') "
        "WHERE ID=?;",
        stmt, result)) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, comp.categoryId);
    sqlite3_bind_text(stmt, 2, comp.partNumber.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, comp.manufacturerId);
    sqlite3_bind_text(stmt, 4, comp.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, comp.notes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, comp.quantity);
    sqlite3_bind_text(stmt, 7, comp.datasheetLink.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 8, comp.id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Delete a component
bool ComponentManager::deleteComponent(int id, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("DELETE FROM Components WHERE ID=?;", stmt, result)) {
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

// List all components
bool ComponentManager::listComponents(std::vector<Component>& comps, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "SELECT ID, CategoryID, PartNumber, ManufacturerID, Description, Notes, "
        "Quantity, DatasheetLink, CreatedOn, ModifiedOn FROM Components;",
        stmt, result)) {
        return false;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Component comp;
        comp.id = sqlite3_column_int(stmt, 0);
        comp.categoryId = sqlite3_column_int(stmt, 1);
        comp.partNumber = safeColumnText(stmt, 2);
        comp.manufacturerId = sqlite3_column_int(stmt, 3);
        comp.description = safeColumnText(stmt, 4);
        comp.notes = safeColumnText(stmt, 5);
        comp.quantity = sqlite3_column_int(stmt, 6);
        comp.datasheetLink = safeColumnText(stmt, 7);
        comp.createdOn = safeColumnText(stmt, 8);
        comp.modifiedOn = safeColumnText(stmt, 9);
        comps.push_back(comp);
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

int ComponentManager::getByPartNumber(const std::string& partNumber, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID FROM Components WHERE PartNumber=?;", stmt, result)) {
        return -1;
    }

    sqlite3_bind_text(stmt, 1, partNumber.c_str(), -1, SQLITE_TRANSIENT);

    int id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        id = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return id;
}
