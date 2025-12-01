#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>

struct FuseType {
    int id{ 0 };
    std::string name;
    FuseType() = default;
    explicit FuseType(const std::string& n) : name(n) {}
};

class FuseTypeManager {
public:
    explicit FuseTypeManager(Database& db);

    bool addType(const FuseType& type, DbResult& res);
    bool getTypeById(int id, FuseType& type, DbResult& res);
    int  getByName(const std::string& name, DbResult& res);
    bool listTypes(std::vector<FuseType>& types, DbResult& res);
    bool deleteType(int id, DbResult& res);

private:
    Database& db_;
};
