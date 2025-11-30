#include "BJTManager.h"

BJTManager::BJTManager(Database& db) : db_(db) {}

bool BJTManager::addBJT(const BJT& bjt, DbResult& result) {
    std::string sql = "INSERT INTO BJTs (ComponentID, VceMax, IcMax, PdMax, Hfe, Ft) VALUES (" +
        std::to_string(bjt.componentId) + "," +
        std::to_string(bjt.vceMax) + "," +
        std::to_string(bjt.icMax) + "," +
        std::to_string(bjt.pdMax) + "," +
        std::to_string(bjt.hfe) + "," +
        std::to_string(bjt.ft) + ");";
    return db_.exec(sql, result);
}

bool BJTManager::getBJTById(int componentId, BJT& bjt, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ComponentID, VceMax, IcMax, PdMax, Hfe, Ft FROM BJTs WHERE ComponentID=?;", stmt, result))
        return false;
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
    db_.finalize(stmt);
    return ok;
}

bool BJTManager::updateBJT(const BJT& bjt, DbResult& result) {
    std::string sql = "UPDATE BJTs SET VceMax=" + std::to_string(bjt.vceMax) +
        ", IcMax=" + std::to_string(bjt.icMax) +
        ", PdMax=" + std::to_string(bjt.pdMax) +
        ", Hfe=" + std::to_string(bjt.hfe) +
        ", Ft=" + std::to_string(bjt.ft) +
        " WHERE ComponentID=" + std::to_string(bjt.componentId) + ";";
    return db_.exec(sql, result);
}

bool BJTManager::deleteBJT(int componentId, DbResult& result) {
    return db_.exec("DELETE FROM BJTs WHERE ComponentID=" + std::to_string(componentId) + ";", result);
}

bool BJTManager::listBJTs(std::vector<BJT>& bjts, DbResult& result) {
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
    return true;
}
