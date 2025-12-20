#pragma once

#include "ComponentManager.h"
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

private:
    explicit InventoryService(std::unique_ptr<Database> db);

    static std::unique_ptr<InventoryService>
        openInternal(const std::string& path, DbResult& result);

    std::unique_ptr<Database> db_;
    ComponentManager componentMgr_;
};
