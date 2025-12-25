#pragma once

#include <string>
#include <vector>

class Database;
class DbResult;
struct LookupItem;

class LookupManager {
public:
    virtual ~LookupManager() = default;

    bool listLookup(std::vector<LookupItem>& items, DbResult& result);
    bool addByName(const std::string& name, DbResult& result);
    int  getIdByName(const std::string& name, DbResult& result);

protected:
    LookupManager(Database& db, const char* tableName);

    Database& db_;
    const char* tableName_;
};
