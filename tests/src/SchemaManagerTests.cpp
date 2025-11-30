#include "BackendTestFixture.h"
#include "SchemaManager.h"

class SchemaManagerTest : public BackendTestFixture {
protected:
    SchemaManager schemaMgr;
    SchemaManagerTest() : schemaMgr(db) {}
};

// 1. Initialize_CreatesAllTables
TEST_F(SchemaManagerTest, Initialize_CreatesAllTables) {
    ASSERT_TRUE(schemaMgr.initialize(res)) << res.toString();

    // Verify baseline tables exist
    EXPECT_TRUE(db.tableExists("Categories"));
    EXPECT_TRUE(db.tableExists("Manufacturers"));
    EXPECT_TRUE(db.tableExists("Components"));
    EXPECT_TRUE(db.tableExists("ResistorComposition"));
    EXPECT_TRUE(db.tableExists("ResistorPackage"));
    EXPECT_TRUE(db.tableExists("Resistors"));

    // Verify SchemaVersion has baseline entry
    int version = db.getMaxSchemaVersion();
    EXPECT_GE(version, 1);
}

// 2. Reinitialize_DoesNotDuplicateTables
TEST_F(SchemaManagerTest, Reinitialize_DoesNotDuplicateTables) {
    ASSERT_TRUE(schemaMgr.initialize(res)) << res.toString();
    ASSERT_TRUE(schemaMgr.initialize(res)) << res.toString();

    // Tables should still exist and not be duplicated
    EXPECT_TRUE(db.tableExists("Components"));
    EXPECT_TRUE(db.tableExists("Resistors"));

    // SchemaVersion should not have duplicate baseline entries
    int count = db.countRows("SchemaVersion", "Version=1");
    EXPECT_EQ(count, 1);
}

// 3. Migration_AddsNewColumns
TEST_F(SchemaManagerTest, Migration_AddsNewColumns) {
    ASSERT_TRUE(schemaMgr.initialize(res)) << res.toString();

    // DatasheetLink column should exist in Components (migration 2)
    EXPECT_TRUE(db.columnExists("Components", "DatasheetLink"));

    // Capacitor-related tables should exist (migration 3)
    EXPECT_TRUE(db.tableExists("CapacitorDielectric"));
    EXPECT_TRUE(db.tableExists("CapacitorPackage"));
    EXPECT_TRUE(db.tableExists("Capacitors"));
    EXPECT_TRUE(db.tableExists("Electrolytics"));

    // SchemaVersion should reflect latest migration
    int version = db.getMaxSchemaVersion();
    EXPECT_GE(version, 3);
}
