#include "Database.h"

Database::Database(const std::string& filename, DbResult& result) : db_(nullptr) {
    int rc = sqlite3_open(filename.c_str(), &db_);
    if (rc) {
        result.setError(rc, sqlite3_errmsg(db_));
        sqlite3_close(db_);
        db_ = nullptr;
    }
    else {
        result.clear();
    }
}

Database::~Database() {
    if (db_) {
        sqlite3_close(db_);
    }
}

bool Database::isOpen() const {
    return db_ != nullptr;
}

bool Database::exec(const std::string& sql, DbResult& result) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string msg = errMsg ? errMsg : "Unknown SQL error";
        sqlite3_free(errMsg);
        result.setError(rc, msg);
        return false;
    }
    result.clear();
    return true;
}

bool Database::prepare(const std::string& sql, sqlite3_stmt*& stmt, DbResult& result) {
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        result.setError(rc, sqlite3_errmsg(db_));
        stmt = nullptr;
        return false;
    }
    result.clear();
    return true;
}

void Database::finalize(sqlite3_stmt* stmt) {
    if (stmt) {
        sqlite3_finalize(stmt);
    }
}

int Database::lastInsertId() const {
    if (db_) {
        return static_cast<int>(sqlite3_last_insert_rowid(db_));
    }
    return 0; // 0 means no valid insert
}

bool Database::tableExists(const std::string& tableName) const {
    if (!db_) return false;

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT name FROM sqlite_master WHERE type='table' AND name=?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_bind_text(stmt, 1, tableName.c_str(), -1, SQLITE_TRANSIENT);

    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = true;
    }

    sqlite3_finalize(stmt);
    return exists;
}

bool Database::columnExists(const std::string& tableName, const std::string& columnName) const {
    if (!db_) return false;

    sqlite3_stmt* stmt = nullptr;
    std::string sql = "PRAGMA table_info(" + tableName + ");";

    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return false;
    }

    bool exists = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* colText = sqlite3_column_text(stmt, 1); // column name is in index 1
        if (colText && columnName == reinterpret_cast<const char*>(colText)) {
            exists = true;
            break;
        }
    }

    sqlite3_finalize(stmt);
    return exists;
}

int Database::getMaxSchemaVersion() const {
    if (!db_) return 0;

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT MAX(Version) FROM SchemaVersion;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return 0;
    }
    int version = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        version = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return version;
}

int Database::countRows(const std::string& tableName, const std::string& whereClause) const {
    if (!db_) return 0;

    std::string sql = "SELECT COUNT(*) FROM " + tableName;
    if (!whereClause.empty()) {
        sql += " WHERE " + whereClause;
    }
    
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return 0;
    }

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count;
}