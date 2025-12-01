#include "BackendTestFixture.h"
#include "FuseManager.h"
#include "ComponentManager.h"
#include "FusePackageManager.h"
#include "FuseTypeManager.h"

class FuseManagerTest : public BackendTestFixture {
protected:
    FuseManager fuseMgr;
    ComponentManager compMgr;
    FusePackageManager pkgMgr;
    FuseTypeManager typeMgr;

    FuseManagerTest()
        : fuseMgr(db), compMgr(db), pkgMgr(db), typeMgr(db) {
    }
};

// 1. AddFuse_InsertsRow
TEST_F(FuseManagerTest, AddFuse_InsertsRow) {
    // Seed a component
    Component c("F1000", "Unit test fuse", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = compMgr.getByPartNumber("F1000", res);

    // Resolve lookup IDs
    int pkgId = pkgMgr.getByName("Axial", res);
    ASSERT_GT(pkgId, 0);
    int typeId = typeMgr.getByName("Fast-blow", res);
    ASSERT_GT(typeId, 0);

    // Insert into Fuses
    Fuse f{ compId, pkgId, typeId };
    EXPECT_TRUE(fuseMgr.addFuse(f, res));

    // Verify retrieval
    Fuse fetched;
    EXPECT_TRUE(fuseMgr.getFuseByComponentId(compId, fetched, res));
    EXPECT_EQ(fetched.componentId, compId);
    EXPECT_EQ(fetched.packageId, pkgId);
    EXPECT_EQ(fetched.typeId, typeId);
}

// 2. DeleteFuse_RemovesRow
TEST_F(FuseManagerTest, DeleteFuse_RemovesRow) {
    Component c("F1001", "Unit test fuse", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = compMgr.getByPartNumber("F1001", res);

    int pkgId = pkgMgr.getByName("Radial", res);
    int typeId = typeMgr.getByName("Slow-blow", res);

    Fuse f{ compId, pkgId, typeId };
    ASSERT_TRUE(fuseMgr.addFuse(f, res));

    EXPECT_TRUE(fuseMgr.deleteFuse(compId, res));

    Fuse fetched;
    EXPECT_FALSE(fuseMgr.getFuseByComponentId(compId, fetched, res));
}

// 3. UpdateFuse_ChangesPersist
TEST_F(FuseManagerTest, UpdateFuse_ChangesPersist) {
    Component c("F1002", "Unit test fuse", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = compMgr.getByPartNumber("F1002", res);

    int pkgId = pkgMgr.getByName("Cartridge", res);
    int typeId = typeMgr.getByName("Fast-blow", res);

    Fuse f{ compId, pkgId, typeId };
    ASSERT_TRUE(fuseMgr.addFuse(f, res));

    // Update to SMD + Resettable
    f.packageId = pkgMgr.getByName("SMD", res);
    f.typeId = typeMgr.getByName("Resettable (polyfuse)", res);
    ASSERT_TRUE(fuseMgr.updateFuse(f, res));

    Fuse fetched;
    ASSERT_TRUE(fuseMgr.getFuseByComponentId(compId, fetched, res));
    EXPECT_EQ(fetched.packageId, f.packageId);
    EXPECT_EQ(fetched.typeId, f.typeId);
}

// 4. ListFuses_ReturnsAll
TEST_F(FuseManagerTest, ListFuses_ReturnsAll) {
    // First component
    Component c1("F1003", "Unit test fuse", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c1, res));
    int compId1 = compMgr.getByPartNumber("F1003", res);

    int pkgId = pkgMgr.getByName("Axial", res);
    int typeId = typeMgr.getByName("Fast-blow", res);
    ASSERT_TRUE(fuseMgr.addFuse(Fuse{ compId1, pkgId, typeId }, res));

    // Second component
    Component c2("F1004", "Unit test fuse", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c2, res));
    int compId2 = compMgr.getByPartNumber("F1004", res);

    ASSERT_TRUE(fuseMgr.addFuse(Fuse{ compId2, pkgId, typeId }, res));

    // Verify list
    std::vector<Fuse> fs;
    EXPECT_TRUE(fuseMgr.listFuses(fs, res));
    EXPECT_EQ(fs.size(), 2);
}
