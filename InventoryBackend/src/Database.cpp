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
