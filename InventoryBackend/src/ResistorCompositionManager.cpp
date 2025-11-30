#include "ResistorCompositionManager.h"
#include "DbUtils.h"
#include <sqlite3.h>

// Add a new composition
bool ResistorCompositionManager::addComposition(const ResistorComposition& comp, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("INSERT INTO ResistorComposition (Name) VALUES (?);", stmt, result))
        return false;

    sqlite3_bind_text(stmt, 1, comp.name.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Get composition by ID
bool ResistorCompositionManager::getCompositionById(int id, ResistorComposition& comp, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name FROM ResistorComposition WHERE ID=?;", stmt, result))
        return false;

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        comp.id = sqlite3_column_int(stmt, 0);
        comp.name = safeColumnText(stmt, 1);
    }
    else {
        result.setError(sqlite3_errcode(db_.handle()), "Composition not found");
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Update composition
bool ResistorCompositionManager::updateComposition(const ResistorComposition& comp, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("UPDATE ResistorComposition SET Name=? WHERE ID=?;", stmt, result))
        return false;

    sqlite3_bind_text(stmt, 1, comp.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, comp.id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Delete composition
bool ResistorCompositionManager::deleteComposition(int id, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("DELETE FROM ResistorComposition WHERE ID=?;", stmt, result))
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

// List all compositions
bool ResistorCompositionManager::listCompositions(std::vector<ResistorComposition>& comps, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name FROM ResistorComposition;", stmt, result))
        return false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ResistorComposition comp;
        comp.id = sqlite3_column_int(stmt, 0);
        comp.name = safeColumnText(stmt, 1);
        comps.push_back(comp);
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

int ResistorCompositionManager::getByName(const std::string& name, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID FROM ResistorComposition WHERE Name=?;", stmt, result)) return -1;
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    int id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return id;
}
