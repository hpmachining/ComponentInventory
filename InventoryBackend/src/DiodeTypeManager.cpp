#include "DiodeTypeManager.h"
#include <sqlite3.h>

DiodeTypeManager::DiodeTypeManager(Database& db) : db_(db) {}

bool DiodeTypeManager::addType(const DiodeType& type, DbResult& res) {
    const char* sql = "INSERT INTO DiodeType (Name) VALUES (?);";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;
    sqlite3_bind_text(stmt, 1, type.name.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok) res.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
    sqlite3_finalize(stmt);
    return ok;
}

bool DiodeTypeManager::getTypeById(int id, DiodeType& type, DbResult& res) {
    const char* sql = "SELECT Id, Name FROM DiodeType WHERE Id=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        type.id = sqlite3_column_int(stmt, 0);
        type.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        sqlite3_finalize(stmt);
        return true;
    }
    sqlite3_finalize(stmt);
    return false;
}

int DiodeTypeManager::getByName(const std::string& name, DbResult& res) {
    const char* sql = "SELECT Id FROM DiodeType WHERE Name=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return -1;
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    int id = (rc == SQLITE_ROW) ? sqlite3_column_int(stmt, 0) : -1;
    sqlite3_finalize(stmt);
    return id;
}

bool DiodeTypeManager::listTypes(std::vector<DiodeType>& types, DbResult& res) {
    const char* sql = "SELECT Id, Name FROM DiodeType;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        DiodeType type;
        type.id = sqlite3_column_int(stmt, 0);
        type.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        types.push_back(type);
    }
    sqlite3_finalize(stmt);
    return true;
}

bool DiodeTypeManager::deleteType(int id, DbResult& res) {
    const char* sql = "DELETE FROM DiodeType WHERE Id=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;
    sqlite3_bind_int(stmt, 1, id);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok) res.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
    sqlite3_finalize(stmt);
    return ok;
}
