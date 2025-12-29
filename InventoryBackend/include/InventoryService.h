#pragma once

#include "ComponentManager.h"
#include "CategoryManager.h"
#include "ManufacturerManager.h"
#include "ResistorManager.h"
#include "ResistorPackageManager.h"
#include "ResistorCompositionManager.h"

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
    ResistorManager& resistors();
    ResistorPackageManager& resistorPackages();
    ResistorCompositionManager& resistorCompositions();

private:
    explicit InventoryService(std::unique_ptr<Database> db);

    static std::unique_ptr<InventoryService>
        openInternal(const std::string& path, DbResult& result);

    std::unique_ptr<Database> db_;
    ComponentManager componentMgr_;
    CategoryManager categoryMgr_;
    ManufacturerManager manufacturerMgr_;
    ResistorManager resistorMgr_;
    ResistorPackageManager resistorPackageMgr_;
    ResistorCompositionManager resistorCompositionMgr_;
};
