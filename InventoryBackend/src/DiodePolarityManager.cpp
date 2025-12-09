#include "DiodePolarityManager.h"
#include <sqlite3.h>

DiodePolarityManager::DiodePolarityManager(Database& db) : db_(db) {}

bool DiodePolarityManager::addPolarity(const DiodePolarity& polarity, DbResult& res) {
    const char* sql = "INSERT INTO DiodePolarity (Name) VALUES (?);";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;
    sqlite3_bind_text(stmt, 1, polarity.name.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok) res.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
    sqlite3_finalize(stmt);
    return ok;
}

bool DiodePolarityManager::getPolarityById(int id, DiodePolarity& polarity, DbResult& res) {
    const char* sql = "SELECT Id, Name FROM DiodePolarity WHERE Id=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        polarity.id = sqlite3_column_int(stmt, 0);
        polarity.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        sqlite3_finalize(stmt);
        return true;
    }
    sqlite3_finalize(stmt);
    return false;
}

int DiodePolarityManager::getByName(const std::string& name, DbResult& res) {
    const char* sql = "SELECT Id FROM DiodePolarity WHERE Name=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return -1;
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    int id = (rc == SQLITE_ROW) ? sqlite3_column_int(stmt, 0) : -1;
    sqlite3_finalize(stmt);
    return id;
}

bool DiodePolarityManager::listPolarities(std::vector<DiodePolarity>& polarities, DbResult& res) {
    const char* sql = "SELECT Id, Name FROM DiodePolarity;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        DiodePolarity polarity;
        polarity.id = sqlite3_column_int(stmt, 0);
        polarity.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        polarities.push_back(polarity);
    }
    sqlite3_finalize(stmt);
    return true;
}

bool DiodePolarityManager::deletePolarity(int id, DbResult& res) {
    const char* sql = "DELETE FROM DiodePolarity WHERE Id=?;";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;
    sqlite3_bind_int(stmt, 1, id);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok) res.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
    sqlite3_finalize(stmt);
    return ok;
}
