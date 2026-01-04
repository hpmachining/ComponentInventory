#pragma once

#include "ComponentManager.h"
#include "CategoryManager.h"
#include "ManufacturerManager.h"
#include "ResistorManager.h"
#include "ResistorPackageManager.h"
#include "ResistorCompositionManager.h"
#include "CapacitorManager.h"
#include "CapacitorPackageManager.h"
#include "CapacitorDielectricManager.h"

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

    ComponentManager& components() { return componentMgr_; }
	CategoryManager& categories() { return categoryMgr_; }
	ManufacturerManager& manufacturers() { return manufacturerMgr_; }
	ResistorManager& resistors() { return resistorMgr_; }
	ResistorPackageManager& resistorPackages() { return resistorPackageMgr_; }
	ResistorCompositionManager& resistorCompositions() { return resistorCompositionMgr_; }
    CapacitorManager& capacitors() { return capacitorManager_; }
    CapacitorPackageManager& capacitorPackages() { return capacitorPackageMgr_; }
    CapacitorDielectricManager& capacitorDielectrics() { return capacitorDielectricMgr_; }
    Database& database() { return *db_; }

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
	CapacitorManager capacitorManager_;
    CapacitorPackageManager capacitorPackageMgr_;
	CapacitorDielectricManager capacitorDielectricMgr_;
};
