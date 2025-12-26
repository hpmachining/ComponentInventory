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

    bool add(const TransistorPolarity& pol, DbResult& result);
    bool getById(int id, TransistorPolarity& pol, DbResult& result);
    bool list(std::vector<TransistorPolarity>& pols, DbResult& result);
    bool remove(int id, DbResult& result);
    int getByName(const std::string& name, DbResult& result);

private:
    Database& db_;
};
