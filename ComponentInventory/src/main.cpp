#include "Database.h"
#include "DbResult.h"
#include "SchemaManager.h"
#include "ConsoleUtils.h"

#include <iostream>

int main() {
    // Force console to UTF-8 output
    configureConsoleUtf8();

    DbResult res;
    Database db("inventory.db", res);
    if (!db.isOpen()) {
        std::cerr << "Failed to open database: " << res.toString() << std::endl;
        return 1;
    }

    SchemaManager schemaMgr(db);
    if (!schemaMgr.initialize(res)) {
        std::cerr << "Failed to initialize schema: " << res.toString() << std::endl;
        return 1;
    }

    return 0;
}
