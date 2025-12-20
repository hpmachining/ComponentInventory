#pragma once
#include "Database.h"
#include "DbResult.h"

class SchemaManager {
public:
    explicit SchemaManager(Database& db) : db_(db) {}

    // Creates tables if missing and upgrades schema if needed.
    // Safe to call for both new and existing databases.
    bool initialize(DbResult& result);

private:
    Database& db_;
};
