#include "LookupManager.h"
#include "Database.h"
#include "DbResult.h"
#include "LookupItem.h"
#include "DbUtils.h"
#include <sqlite3.h>

LookupManager::LookupManager(Database& db, const char* tableName)
    : db_(db), tableName_(tableName)
{
}

bool LookupManager::listLookup(std::vector<LookupItem>& items, DbResult& result)
{
    result.clear();
    items.clear();

    std::string sql = std::string("SELECT ID, Name FROM ") + tableName_ + " ORDER BY Name;";

    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql.c_str(), stmt, result))
        return false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        LookupItem item;
        item.id = sqlite3_column_int(stmt, 0);
        const unsigned char* txt = sqlite3_column_text(stmt, 1);
        item.name = txt ? reinterpret_cast<const char*>(txt) : std::string();
        items.push_back(std::move(item));
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

int LookupManager::getIdByName(const std::string& name, DbResult& result)
{
    result.clear();

    std::string sql = std::string("SELECT ID FROM ") + tableName_ + " WHERE Name = ? LIMIT 1;";

    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql.c_str(), stmt, result))
        return -1;

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    int id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        id = sqlite3_column_int(stmt, 0);
    }

    db_.finalize(stmt);
    return id;
}

bool LookupManager::addByName(const std::string& rawName, DbResult& result)
{
    result.clear();

    // Trim and validate
    const std::string name = normalizeWhitespace(rawName);
    if (name.empty()) {
        result.setError(
            LookupError::EmptyName,            // uses your DbResult enum overload
            "Name cannot be empty"
        );
        return false;
    }

    // Pre-check existence (uses COLLATE NOCASE in schema, or explicit COLLATE if needed)
    int existing = getIdByName(name, result);
    if (result.ok() && existing >= 0) {
        result.setError(
            LookupError::AlreadyExists,
            "Entry already exists"
        );
        return false;
    }

    // Insert directly into the lookup table
    std::string sql = std::string("INSERT INTO ") + tableName_ + " (Name) VALUES (?);";
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare(sql.c_str(), stmt, result))
        return false;

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        // Map unique constraint to AlreadyExists, otherwise surface DB error
        int err = sqlite3_extended_errcode(db_.handle()); // full extended code
        db_.finalize(stmt);

        if (err == SQLITE_CONSTRAINT || err == SQLITE_CONSTRAINT_UNIQUE) {
            result.setError(
                LookupError::AlreadyExists,
                "Entry already exists"
            );
            return false;
        }

        result.setError(err, sqlite3_errmsg(db_.handle()));
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}
