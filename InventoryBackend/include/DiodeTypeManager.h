#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>

struct DiodeType {
    int id{ 0 };
    std::string name;
    DiodeType() = default;
    explicit DiodeType(const std::string& n) : name(n) {}
};

class DiodeTypeManager {
public:
    explicit DiodeTypeManager(Database& db);

    bool addType(const DiodeType& type, DbResult& res);
    bool getTypeById(int id, DiodeType& type, DbResult& res);
    int  getByName(const std::string& name, DbResult& res);
    bool listTypes(std::vector<DiodeType>& types, DbResult& res);
    bool deleteType(int id, DbResult& res);

private:
    Database& db_;
};
