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

    bool add(const DiodePolarity& polarity, DbResult& res);
    bool getById(int id, DiodePolarity& polarity, DbResult& res);
    int  getByName(const std::string& name, DbResult& res);
    bool list(std::vector<DiodePolarity>& polarities, DbResult& res);
    bool remove(int id, DbResult& res);

private:
    Database& db_;
};
