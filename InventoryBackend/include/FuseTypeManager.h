#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>
#include <string>

struct FuseType {
    int id;
    std::string name;

    FuseType() : id(0), name() {}
    explicit FuseType(const std::string& n) : id(0), name(n) {}
    FuseType(int id, const std::string& n) : id(id), name(n) {}
};

class FuseTypeManager {
public:
    explicit FuseTypeManager(Database& db);

    bool add(const FuseType& type, DbResult& res);
    bool getById(int id, FuseType& type, DbResult& res);
    int  getByName(const std::string& name, DbResult& res);
    bool list(std::vector<FuseType>& types, DbResult& res);
    bool remove(int id, DbResult& res);

private:
    Database& db_;
};
