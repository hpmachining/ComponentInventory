#pragma once

#include "LookupManager.h"
#include "Database.h"
#include "DbResult.h"
#include <vector>
#include <string>

struct Category {
    int id;                  // Primary key
    std::string name;        // e.g. "Resistor"
    std::string description; // Optional longer description

    Category() : id(0) {}
    Category(const std::string& n, const std::string& d = "")
        : id(0), name(n), description(d) {
    }
};

class CategoryManager : public LookupManager {
public:
    explicit CategoryManager(Database& db);

    bool add(const Category& cat, DbResult& result);
    bool getById(int id, Category& cat, DbResult& result);
    bool update(const Category& cat, DbResult& result);
    bool remove(int id, DbResult& result);
    bool list(std::vector<Category>& cats, DbResult& result);
};
