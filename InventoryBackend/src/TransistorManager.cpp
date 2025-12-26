#include "TransistorManager.h"

TransistorManager::TransistorManager(Database& db)
    : db_(db) {
}

bool TransistorManager::add(const Transistor& t, DbResult& result) {
    std::string sql =
        "INSERT INTO Transistors (ComponentID, TypeID, PolarityID, PackageID) VALUES (" +
        std::to_string(t.componentId) + "," +
        std::to_string(t.typeId) + "," +
        std::to_string(t.polarityId) + "," +
        std::to_string(t.packageId) + ");";

    return db_.exec(sql, result);
}

bool TransistorManager::getById(int componentId, Transistor& t, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "SELECT ComponentID, TypeID, PolarityID, PackageID "
        "FROM Transistors WHERE ComponentID=?;",
        stmt, result))
        return false;

    sqlite3_bind_int(stmt, 1, componentId);

    bool ok = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        t.componentId = sqlite3_column_int(stmt, 0);
        t.typeId = sqlite3_column_int(stmt, 1);
        t.polarityId = sqlite3_column_int(stmt, 2);
        t.packageId = sqlite3_column_int(stmt, 3);
        ok = true;
    }

    db_.finalize(stmt);
    return ok;
}

bool TransistorManager::list(std::vector<Transistor>& ts, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(
        "SELECT ComponentID, TypeID, PolarityID, PackageID FROM Transistors;",
        stmt, result))
        return false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Transistor t;
        t.componentId = sqlite3_column_int(stmt, 0);
        t.typeId = sqlite3_column_int(stmt, 1);
        t.polarityId = sqlite3_column_int(stmt, 2);
        t.packageId = sqlite3_column_int(stmt, 3);
        ts.push_back(t);
    }

    db_.finalize(stmt);
    return true;
}

bool TransistorManager::remove(int componentId, DbResult& result) {
    return db_.exec(
        "DELETE FROM Transistors WHERE ComponentID=" +
        std::to_string(componentId) + ";",
        result);
}
