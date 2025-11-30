#include "TransistorPolarityManager.h"

TransistorPolarityManager::TransistorPolarityManager(Database& db) : db_(db) {}

bool TransistorPolarityManager::addPolarity(const TransistorPolarity& pol, DbResult& result) {
    return db_.exec("INSERT INTO TransistorPolarity (Name) VALUES ('" + pol.name + "');", result);
}

bool TransistorPolarityManager::getPolarityById(int id, TransistorPolarity& pol, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name FROM TransistorPolarity WHERE ID=?;", stmt, result)) return false;
    sqlite3_bind_int(stmt, 1, id);

    bool ok = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        pol.id = sqlite3_column_int(stmt, 0);
        pol.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        ok = true;
    }
    db_.finalize(stmt);
    return ok;
}

bool TransistorPolarityManager::listPolarities(std::vector<TransistorPolarity>& pols, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name FROM TransistorPolarity;", stmt, result)) return false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        TransistorPolarity p;
        p.id = sqlite3_column_int(stmt, 0);
        p.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        pols.push_back(p);
    }
    db_.finalize(stmt);
    return true;
}

bool TransistorPolarityManager::deletePolarity(int id, DbResult& result) {
    return db_.exec("DELETE FROM TransistorPolarity WHERE ID=" + std::to_string(id) + ";", result);
}

int TransistorPolarityManager::getByName(const std::string& name, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID FROM TransistorPolarity WHERE Name=?;", stmt, result)) return -1;
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    int id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return id;
}
