#include "TransistorTypeManager.h"

TransistorTypeManager::TransistorTypeManager(Database& db) : db_(db) {}

// Implementations will use db_.exec / db_.prepare to perform CRUD
bool TransistorTypeManager::addType(const TransistorType& type, DbResult& result) {
    return db_.exec("INSERT INTO TransistorType (Name) VALUES ('" + type.name + "');", result);
}

bool TransistorTypeManager::getTypeById(int id, TransistorType& type, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name FROM TransistorType WHERE ID=?;", stmt, result)) return false;
    sqlite3_bind_int(stmt, 1, id);
    bool ok = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        type.id = sqlite3_column_int(stmt, 0);
        type.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        ok = true;
    }
    db_.finalize(stmt);
    return ok;
}

bool TransistorTypeManager::listTypes(std::vector<TransistorType>& types, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name FROM TransistorType;", stmt, result)) return false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        TransistorType t;
        t.id = sqlite3_column_int(stmt, 0);
        t.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        types.push_back(t);
    }
    db_.finalize(stmt);
    return true;
}

bool TransistorTypeManager::deleteType(int id, DbResult& result) {
    return db_.exec("DELETE FROM TransistorType WHERE ID=" + std::to_string(id) + ";", result);
}
