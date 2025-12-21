#pragma once

#include "ComponentManager.h"
#include "CategoryManager.h"
#include "ManufacturerManager.h"
#include <memory>
#include <string>

class Database;
class DbResult;

class InventoryService {
public:
    static std::unique_ptr<InventoryService>
        open(const std::string& path, DbResult& result);

    static std::unique_ptr<InventoryService>
        create(const std::string& path, DbResult& result);

    ComponentManager& components();
    CategoryManager& categories();
    ManufacturerManager& manufacturers();

private:
    explicit InventoryService(std::unique_ptr<Database> db);

    static std::unique_ptr<InventoryService>
        openInternal(const std::string& path, DbResult& result);

    std::unique_ptr<Database> db_;
    ComponentManager componentMgr_;
    CategoryManager categoryMgr_;
    ManufacturerManager manufacturerMgr_;
};
