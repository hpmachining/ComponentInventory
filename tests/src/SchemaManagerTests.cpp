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

    // Baseline tables
    EXPECT_TRUE(db.tableExists("Categories"));
    EXPECT_TRUE(db.tableExists("Manufacturers"));
    EXPECT_TRUE(db.tableExists("Components"));
    EXPECT_TRUE(db.tableExists("ResistorComposition"));
    EXPECT_TRUE(db.tableExists("ResistorPackage"));
    EXPECT_TRUE(db.tableExists("Resistors"));

    // Audit fields
    EXPECT_TRUE(db.columnExists("Components", "CreatedOn"));
    EXPECT_TRUE(db.columnExists("Components", "ModifiedOn"));

    // SchemaVersion baseline
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

    // Migration 2: DatasheetLink
    EXPECT_TRUE(db.columnExists("Components", "DatasheetLink"));

    // Migration 3: Capacitor tables
    EXPECT_TRUE(db.tableExists("CapacitorDielectric"));
    EXPECT_TRUE(db.tableExists("CapacitorPackage"));
    EXPECT_TRUE(db.tableExists("Capacitors"));
    EXPECT_TRUE(db.tableExists("Electrolytics"));

    // Migration 4: Transistor tables
    EXPECT_TRUE(db.tableExists("TransistorType"));
    EXPECT_TRUE(db.tableExists("TransistorPolarity"));
    EXPECT_TRUE(db.tableExists("TransistorPackage"));
    EXPECT_TRUE(db.tableExists("Transistors"));
    EXPECT_TRUE(db.tableExists("BJTs"));

    // Migration 5: Seeds should exist
    EXPECT_TRUE(db.rowExists("Categories", "Name='Fuse'", res));
    EXPECT_TRUE(db.rowExists("Manufacturers", "Name='Rubycon'", res));
    EXPECT_TRUE(db.rowExists("ResistorComposition", "Name='Carbon Film'", res));
    EXPECT_TRUE(db.rowExists("ResistorPackage", "Name='Axial leaded'", res));
    EXPECT_TRUE(db.rowExists("CapacitorDielectric", "Name='C0G/NP0'", res));
    EXPECT_TRUE(db.rowExists("CapacitorPackage", "Name='Radial leaded'", res));
    EXPECT_TRUE(db.rowExists("TransistorType", "Name='MOSFET'", res));
    EXPECT_TRUE(db.rowExists("TransistorPolarity", "Name='NPN'", res));
    EXPECT_TRUE(db.rowExists("TransistorPackage", "Name='TO-92'", res));

    // Migration 6: Fuse tables and seeds
    EXPECT_TRUE(db.tableExists("FusePackage"));
    EXPECT_TRUE(db.tableExists("FuseType"));
    EXPECT_TRUE(db.tableExists("Fuses"));
    EXPECT_TRUE(db.rowExists("FusePackage", "Name='Axial'", res));
    EXPECT_TRUE(db.rowExists("FusePackage", "Name='Radial'", res));
    EXPECT_TRUE(db.rowExists("FusePackage", "Name='Cartridge'", res));
    EXPECT_TRUE(db.rowExists("FusePackage", "Name='SMD'", res));
    EXPECT_TRUE(db.rowExists("FuseType", "Name='Fast-blow'", res));
    EXPECT_TRUE(db.rowExists("FuseType", "Name='Slow-blow'", res));
    EXPECT_TRUE(db.rowExists("FuseType", "Name='Resettable (polyfuse)'", res));

    // New: Fuse ratings columns
    EXPECT_TRUE(db.columnExists("Fuses", "CurrentRating"));
    EXPECT_TRUE(db.columnExists("Fuses", "VoltageRating"));

    // SchemaVersion should reflect latest migration
    int version = db.getMaxSchemaVersion();
    EXPECT_GE(version, 6);
}
