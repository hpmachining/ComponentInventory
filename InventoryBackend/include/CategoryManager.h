#pragma once
#include "Database.h"
#include "DbResult.h"
#include "Category.h"
#include <vector>

class CategoryManager {
public:
    CategoryManager(Database& db) : db_(db) {}

    bool addCategory(const Category& cat, DbResult& result);
    bool getCategoryById(int id, Category& cat, DbResult& result);
    bool updateCategory(const Category& cat, DbResult& result);
    bool deleteCategory(int id, DbResult& result);
    bool listCategories(std::vector<Category>& cats, DbResult& result);

private:
    Database& db_;
};
