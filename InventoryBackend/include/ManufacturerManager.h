#pragma once
#include "Database.h"
#include "DbResult.h"
#include "Manufacturer.h"
#include "LookupItem.h"
#include "LookupManager.h"

#include <vector>

class ManufacturerManager : public LookupManager {
public:
    ManufacturerManager(Database& db)
        : LookupManager(db, "Manufacturers") {
    }

    bool add(const Manufacturer& man, DbResult& result);
    bool getById(int id, Manufacturer& man, DbResult& result);
    bool update(const Manufacturer& man, DbResult& result);
    bool remove(int id, DbResult& result);
    bool list(std::vector<Manufacturer>& mans, DbResult& result);
};
