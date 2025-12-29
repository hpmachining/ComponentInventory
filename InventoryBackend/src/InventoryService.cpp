#include "InventoryService.h"

#include "Database.h"
#include "SchemaManager.h"
#include "ComponentManager.h"
#include "DbResult.h"

// ---- Construction ----

InventoryService::InventoryService(std::unique_ptr<Database> db)
    : db_(std::move(db))
    , componentMgr_(*db_)
    , categoryMgr_(*db_)
    , manufacturerMgr_(*db_)
    , resistorMgr_(*db_)
    , resistorPackageMgr_(*db_)
    , resistorCompositionMgr_(*db_)
{
}

// ---- Factory methods ----

std::unique_ptr<InventoryService>
InventoryService::open(const std::string& path, DbResult& result)
{
    return openInternal(path, result);
}

std::unique_ptr<InventoryService>
InventoryService::create(const std::string& path, DbResult& result)
{
    // Future differentiation point:
    // - ensure file does not exist
    // - delete existing file
    // - call schema.createFresh()

    return openInternal(path, result);
}

std::unique_ptr<InventoryService>
InventoryService::openInternal(const std::string& path, DbResult& result)
{
    auto db = std::make_unique<Database>(path, result);
    if (!db->isOpen())
        return nullptr;

    SchemaManager schema(*db);
    if (!schema.initialize(result))
        return nullptr;

    return std::unique_ptr<InventoryService>(
        new InventoryService(std::move(db))
    );
}

// ---- Accessors ----

ComponentManager& InventoryService::components()
{
    return componentMgr_;
}

CategoryManager& InventoryService::categories()
{
    return categoryMgr_;
}

ManufacturerManager& InventoryService::manufacturers()
{
    return manufacturerMgr_;
}

ResistorManager& InventoryService::resistors()
{
    return resistorMgr_;
}

ResistorPackageManager& InventoryService::resistorPackages()
{
    return resistorPackageMgr_;
}

ResistorCompositionManager& InventoryService::resistorCompositions()
{
    return resistorCompositionMgr_;
}
