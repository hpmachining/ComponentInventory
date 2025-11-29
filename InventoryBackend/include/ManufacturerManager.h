#pragma once
#include "Database.h"
#include "DbResult.h"
#include "Manufacturer.h"
#include <vector>

class ManufacturerManager {
public:
    ManufacturerManager(Database& db) : db_(db) {}

    bool addManufacturer(const Manufacturer& man, DbResult& result);
    bool getManufacturerById(int id, Manufacturer& man, DbResult& result);
    bool updateManufacturer(const Manufacturer& man, DbResult& result);
    bool deleteManufacturer(int id, DbResult& result);
    bool listManufacturers(std::vector<Manufacturer>& mans, DbResult& result);

private:
    Database& db_;
};
