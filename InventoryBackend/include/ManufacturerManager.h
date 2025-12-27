#pragma once
#include "Database.h"
#include "DbResult.h"
#include "LookupItem.h"
#include "LookupManager.h"

#include <vector>
#include <string>

struct Manufacturer {
    int id;                  // Primary key
    std::string name;        // e.g. "Vishay"
    std::string country;     // Optional
    std::string website;     // Optional
    std::string notes;       // Optional

    Manufacturer() : id(0) {}
    Manufacturer(const std::string& n,
        const std::string& c = "",
        const std::string& w = "",
        const std::string& note = "")
        : id(0), name(n), country(c), website(w), notes(note) {
    }
};

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
