#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>

struct DiodePolarity {
    int id{ 0 };
    std::string name;
    DiodePolarity() = default;
    explicit DiodePolarity(const std::string& n) : name(n) {}
};

class DiodePolarityManager {
public:
    explicit DiodePolarityManager(Database& db);

    bool addPolarity(const DiodePolarity& pol, DbResult& res);
    bool getPolarityById(int id, DiodePolarity& pol, DbResult& res);
    int  getByName(const std::string& name, DbResult& res);
    bool listPolarities(std::vector<DiodePolarity>& pols, DbResult& res);
    bool deletePolarity(int id, DbResult& res);

private:
    Database& db_;
};
