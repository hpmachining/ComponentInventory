#pragma once

#include "LookupManager.h"
#include "Database.h"
#include "DbResult.h"
#include "Category.h"

#include <vector>

class CategoryManager : public LookupManager {
public:
    explicit CategoryManager(Database& db);

    bool add(const Category& cat, DbResult& result);
    bool getById(int id, Category& cat, DbResult& result);
    bool update(const Category& cat, DbResult& result);
    bool remove(int id, DbResult& result);
    bool list(std::vector<Category>& cats, DbResult& result);
};
