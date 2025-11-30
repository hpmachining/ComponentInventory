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

    // Baseline audit fields
    EXPECT_TRUE(db.columnExists("Components", "CreatedOn"));
    EXPECT_TRUE(db.columnExists("Components", "ModifiedOn"));

    // Verify SchemaVersion has baseline entry
    int version = db.getMaxSchemaVersion();
    EXPECT_GE(version, 1);
}

// 2. Reinitialize_DoesNotDuplicateTables
TEST_F(SchemaManagerTest, Reinitialize_DoesNotDuplicateTables) {
    ASSERT_TRUE(schemaMgr.initialize(res)) << res.toString();
    ASSERT_TRUE(schemaMgr.initialize(res)) << res.toString();

    EXPECT_TRUE(db.tableExists("Components"));
    EXPECT_TRUE(db.tableExists("Resistors"));

    int count = db.countRows("SchemaVersion", "Version=1");
    EXPECT_EQ(count, 1);
}

// 3. Migration_AddsNewColumnsAndTables
TEST_F(SchemaManagerTest, Migration_AddsNewColumnsAndTables) {
    ASSERT_TRUE(schemaMgr.initialize(res)) << res.toString();

    // Migration 2: DatasheetLink column
    EXPECT_TRUE(db.columnExists("Components", "DatasheetLink"));

    // Migration 3: Capacitor-related tables
    EXPECT_TRUE(db.tableExists("CapacitorDielectric"));
    EXPECT_TRUE(db.tableExists("CapacitorPackage"));
    EXPECT_TRUE(db.tableExists("Capacitors"));
    EXPECT_TRUE(db.tableExists("Electrolytics"));

    // Migration 4: Transistor-related tables
    EXPECT_TRUE(db.tableExists("TransistorType"));
    EXPECT_TRUE(db.tableExists("TransistorPolarity"));
    EXPECT_TRUE(db.tableExists("TransistorPackage"));
    EXPECT_TRUE(db.tableExists("Transistors"));
    EXPECT_TRUE(db.tableExists("BJTs"));

    // SchemaVersion should reflect latest migration
    int version = db.getMaxSchemaVersion();
    EXPECT_GE(version, 4);
}
