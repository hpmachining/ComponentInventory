#include "CategoryManager.h"
#include "DbUtils.h"
#include <sqlite3.h>

// Add a new category
bool CategoryManager::addCategory(const Category& cat, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("INSERT INTO Categories (Name, Description) VALUES (?, ?);", stmt, result))
        return false;

    sqlite3_bind_text(stmt, 1, cat.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, cat.description.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Get category by ID
bool CategoryManager::getCategoryById(int id, Category& cat, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name, Description FROM Categories WHERE ID=?;", stmt, result))
        return false;

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        cat.id = sqlite3_column_int(stmt, 0);
        cat.name = safeColumnText(stmt, 1);
        cat.description = safeColumnText(stmt, 2);
    }
    else {
        result.setError(sqlite3_errcode(db_.handle()), "Category not found");
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Update category
bool CategoryManager::updateCategory(const Category& cat, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("UPDATE Categories SET Name=?, Description=? WHERE ID=?;", stmt, result))
        return false;

    sqlite3_bind_text(stmt, 1, cat.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, cat.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, cat.id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// Delete category
bool CategoryManager::deleteCategory(int id, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("DELETE FROM Categories WHERE ID=?;", stmt, result))
        return false;

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
        db_.finalize(stmt);
        return false;
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}

// List all categories
bool CategoryManager::listCategories(std::vector<Category>& cats, DbResult& result) {
    sqlite3_stmt* stmt = nullptr;
    if (!db_.prepare("SELECT ID, Name, Description FROM Categories;", stmt, result))
        return false;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Category cat;
        cat.id = sqlite3_column_int(stmt, 0);
        cat.name = safeColumnText(stmt, 1);
        cat.description = safeColumnText(stmt, 2);
        cats.push_back(cat);
    }

    db_.finalize(stmt);
    result.clear();
    return true;
}
