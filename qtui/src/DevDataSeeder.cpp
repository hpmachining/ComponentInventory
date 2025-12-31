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

    // Create named Component objects so add can update comp.id
    Component c1("PN123", "Test component", catId, manId, 10, "Notes");
    if (!compMgr.add(c1, res)) {
        // handle error or at least log / ignore for test seeding
    }

    Component c2("PN456", "Another test component", catId, manId, 5, "More notes");
    if (!compMgr.add(c2, res)) {
        // handle error
    }

    Component c3("PN789", "To be updated", catId, manId, 1, "Old notes");
    if (!compMgr.add(c3, res)) {
        // handle error
    }
}

