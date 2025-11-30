#pragma once
#include <string>
#include <sqlite3.h>
#include "DbResult.h"

class Database {
public:
    Database(const std::string& filename, DbResult& result);
    ~Database();

    bool isOpen() const;

    // Execute a SQL statement (no results expected)
    bool exec(const std::string& sql, DbResult& result);

    // Prepare a statement for parameterized queries
    bool prepare(const std::string& sql, sqlite3_stmt*& stmt, DbResult& result);

    // Finalize a prepared statement
    void finalize(sqlite3_stmt* stmt);

    sqlite3* handle() const { return db_; }
    int lastInsertId() const;
    bool tableExists(const std::string& tableName) const;
	bool columnExists(const std::string& tableName, const std::string& columnName) const;
    int getMaxSchemaVersion() const;
	int countRows(const std::string& tableName, const std::string& whereClause) const;

private:
    sqlite3* db_;
};
