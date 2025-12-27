#include "FuseManager.h"
#include <sqlite3.h>

FuseManager::FuseManager(Database& db)
    : db_(db) {
}

bool FuseManager::add(const Fuse& fuse, DbResult& res) {
    const char* sql =
        "INSERT INTO Fuses "
        "(ComponentId, PackageId, TypeId, CurrentRating, VoltageRating) "
        "VALUES (?,?,?,?,?);";

    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;

    sqlite3_bind_int(stmt, 1, fuse.componentId);
    sqlite3_bind_int(stmt, 2, fuse.packageId);
    sqlite3_bind_int(stmt, 3, fuse.typeId);
    sqlite3_bind_double(stmt, 4, fuse.currentRating);
    sqlite3_bind_double(stmt, 5, fuse.voltageRating);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok)
        res.setError(sqlite3_errcode(db_.handle()),
            sqlite3_errmsg(db_.handle()));

    sqlite3_finalize(stmt);
    return ok;
}

bool FuseManager::getById(int componentId, Fuse& fuse, DbResult& res) {
    const char* sql =
        "SELECT ComponentId, PackageId, TypeId, CurrentRating, VoltageRating "
        "FROM Fuses WHERE ComponentId=?;";

    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;

    sqlite3_bind_int(stmt, 1, componentId);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        fuse.componentId = sqlite3_column_int(stmt, 0);
        fuse.packageId = sqlite3_column_int(stmt, 1);
        fuse.typeId = sqlite3_column_int(stmt, 2);
        fuse.currentRating = sqlite3_column_double(stmt, 3);
        fuse.voltageRating = sqlite3_column_double(stmt, 4);
        sqlite3_finalize(stmt);
        return true;
    }

    sqlite3_finalize(stmt);
    return false;
}

bool FuseManager::update(const Fuse& fuse, DbResult& res) {
    const char* sql =
        "UPDATE Fuses SET "
        "PackageId=?, TypeId=?, CurrentRating=?, VoltageRating=? "
        "WHERE ComponentId=?;";

    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;

    sqlite3_bind_int(stmt, 1, fuse.packageId);
    sqlite3_bind_int(stmt, 2, fuse.typeId);
    sqlite3_bind_double(stmt, 3, fuse.currentRating);
    sqlite3_bind_double(stmt, 4, fuse.voltageRating);
    sqlite3_bind_int(stmt, 5, fuse.componentId);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok)
        res.setError(sqlite3_errcode(db_.handle()),
            sqlite3_errmsg(db_.handle()));

    sqlite3_finalize(stmt);
    return ok;
}

bool FuseManager::remove(int componentId, DbResult& res) {
    const char* sql = "DELETE FROM Fuses WHERE ComponentId=?;";

    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;

    sqlite3_bind_int(stmt, 1, componentId);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok)
        res.setError(sqlite3_errcode(db_.handle()),
            sqlite3_errmsg(db_.handle()));

    sqlite3_finalize(stmt);
    return ok;
}

bool FuseManager::list(std::vector<Fuse>& fuses, DbResult& res) {
    const char* sql =
        "SELECT ComponentId, PackageId, TypeId, CurrentRating, VoltageRating "
        "FROM Fuses;";

    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql, stmt, res)) return false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Fuse f;
        f.componentId = sqlite3_column_int(stmt, 0);
        f.packageId = sqlite3_column_int(stmt, 1);
        f.typeId = sqlite3_column_int(stmt, 2);
        f.currentRating = sqlite3_column_double(stmt, 3);
        f.voltageRating = sqlite3_column_double(stmt, 4);
        fuses.push_back(f);
    }

    sqlite3_finalize(stmt);
    return true;
}
