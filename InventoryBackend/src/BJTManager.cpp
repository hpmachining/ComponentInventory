#include "BJTManager.h"
#include "DbUtils.h"
#include <sqlite3.h>

BJTManager::BJTManager(Database& db) : db_(db) {}

// Add BJT with prepared statement
bool BJTManager::add(const BJT& bjt, DbResult& result) {
    // Optional: validate component exists
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "INSERT INTO BJTs (ComponentID, VceMax, IcMax, PdMax, Hfe, Ft) "
        "VALUES (?, ?, ?, ?, ?, ?);",
        stmt, result)) return false;

    sqlite3_bind_int(stmt, 1, bjt.componentId);
    sqlite3_bind_double(stmt, 2, bjt.vceMax);
    sqlite3_bind_double(stmt, 3, bjt.icMax);
    sqlite3_bind_double(stmt, 4, bjt.pdMax);
    sqlite3_bind_double(stmt, 5, bjt.hfe);
    sqlite3_bind_double(stmt, 6, bjt.ft);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Get BJT by ComponentID
bool BJTManager::getById(int componentId, BJT& bjt, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "SELECT ComponentID, VceMax, IcMax, PdMax, Hfe, Ft FROM BJTs WHERE ComponentID=?;",
        stmt, result)) return false;

    sqlite3_bind_int(stmt, 1, componentId);

    bool ok = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        bjt.componentId = sqlite3_column_int(stmt, 0);
        bjt.vceMax = sqlite3_column_double(stmt, 1);
        bjt.icMax = sqlite3_column_double(stmt, 2);
        bjt.pdMax = sqlite3_column_double(stmt, 3);
        bjt.hfe = sqlite3_column_double(stmt, 4);
        bjt.ft = sqlite3_column_double(stmt, 5);
        ok = true;
    }
    else {
        result.setError(sqlite3_errcode(db_.handle()), "BJT not found");
    }

    db_.finalize(stmt);
    return ok;
}

// Update BJT
bool BJTManager::update(const BJT& bjt, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "UPDATE BJTs SET VceMax=?, IcMax=?, PdMax=?, Hfe=?, Ft=? WHERE ComponentID=?;",
        stmt, result)) return false;

    sqlite3_bind_double(stmt, 1, bjt.vceMax);
    sqlite3_bind_double(stmt, 2, bjt.icMax);
    sqlite3_bind_double(stmt, 3, bjt.pdMax);
    sqlite3_bind_double(stmt, 4, bjt.hfe);
    sqlite3_bind_double(stmt, 5, bjt.ft);
    sqlite3_bind_int(stmt, 6, bjt.componentId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Remove BJT
bool BJTManager::remove(int componentId, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("DELETE FROM BJTs WHERE ComponentID=?;", stmt, result)) return false;
    sqlite3_bind_int(stmt, 1, componentId);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// List all BJTs
bool BJTManager::list(std::vector<BJT>& bjts, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ComponentID, VceMax, IcMax, PdMax, Hfe, Ft FROM BJTs;", stmt, result))
        return false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        BJT b;
        b.componentId = sqlite3_column_int(stmt, 0);
        b.vceMax = sqlite3_column_double(stmt, 1);
        b.icMax = sqlite3_column_double(stmt, 2);
        b.pdMax = sqlite3_column_double(stmt, 3);
        b.hfe = sqlite3_column_double(stmt, 4);
        b.ft = sqlite3_column_double(stmt, 5);
        bjts.push_back(b);
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Optional: Lookup for GUI dropdowns
bool BJTManager::listLookup(std::vector<LookupItem>& items, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "SELECT c.ID, c.PartNumber FROM Components c "
        "INNER JOIN BJTs b ON c.ID = b.ComponentID;", stmt, result)) return false;

    items.clear();
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        items.push_back({ sqlite3_column_int(stmt, 0), safeColumnText(stmt, 1) });
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}
