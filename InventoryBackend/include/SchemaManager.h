#pragma once
#include "Database.h"
#include "DbResult.h"

class SchemaManager {
public:
    explicit SchemaManager(Database& db) : db_(db) {}

    // Initialize or upgrade schema
    bool initialize(DbResult& result);

private:
    Database& db_;
};
