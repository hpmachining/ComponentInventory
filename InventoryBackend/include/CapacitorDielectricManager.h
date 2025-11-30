#pragma once

#include "Database.h"
#include "DbResult.h"
#include <vector>
#include <string>

// Lookup struct
struct CapacitorDielectric {
    int id;
    std::string name;

    CapacitorDielectric() : id(0), name("") {}
    CapacitorDielectric(const std::string& n) : id(0), name(n) {}
};

class CapacitorDielectricManager {
public:
    explicit CapacitorDielectricManager(Database& db);

    bool addDielectric(const CapacitorDielectric& diel, DbResult& result);
    bool getDielectricById(int id, CapacitorDielectric& diel, DbResult& result);
    bool listDielectrics(std::vector<CapacitorDielectric>& diels, DbResult& result);
    bool deleteDielectric(int id, DbResult& result);
	int getByName(const std::string& name, DbResult& result);

private:
    Database& db_;
};
