#pragma once
#include "Database.h"
#include "DbResult.h"
#include <string>
#include <vector>

struct TransistorPolarity {
    int id;
    std::string name;

    TransistorPolarity() : id(0), name("") {}
    explicit TransistorPolarity(const std::string& n) : id(0), name(n) {}
};

class TransistorPolarityManager {
public:
    explicit TransistorPolarityManager(Database& db);

    bool addPolarity(const TransistorPolarity& pol, DbResult& result);
    bool getPolarityById(int id, TransistorPolarity& pol, DbResult& result);
    bool listPolarities(std::vector<TransistorPolarity>& pols, DbResult& result);
    bool deletePolarity(int id, DbResult& result);

private:
    Database& db_;
};
