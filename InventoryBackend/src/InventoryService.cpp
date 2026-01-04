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
	, capacitorManager_(*db_)
	, capacitorPackageMgr_(*db_)
	, capacitorDielectricMgr_(*db_)
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
