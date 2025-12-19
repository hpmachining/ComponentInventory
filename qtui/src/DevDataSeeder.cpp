#include "DevDataSeeder.h"
#include "ComponentManager.h"
#include "DbResult.h"

void DevDataSeeder::seedComponents(Database& db)
{
    DbResult res;
    ComponentManager compMgr(db);

    // Guard: do not reseed if data exists
    if (db.countRows("Components", "") > 0)
        return;

    // These IDs must exist (Categories, Manufacturers)
    // You already create them in SchemaManager / test fixture
    int catId = 1;
    int manId = 1;

    compMgr.addComponent(
        Component("PN123", "Test component", catId, manId, 10, "Notes"),
        res);

    compMgr.addComponent(
        Component("PN456", "Another test component", catId, manId, 5, "More notes"),
        res);

    compMgr.addComponent(
        Component("PN789", "To be updated", catId, manId, 1, "Old notes"),
        res);
}
