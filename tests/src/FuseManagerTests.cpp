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

    int pkgId{ 0 };
    int fastTypeId{ 0 };
    int slowTypeId{ 0 };

    FuseManagerTest()
        : fuseMgr(db),
        compMgr(db),
        pkgMgr(db),
        typeMgr(db)
    {
    }

    void SetUp() override {
        BackendTestFixture::SetUp();

        // Create a package
        FusePackage pkg("TEST_AXIAL");
        ASSERT_TRUE(pkgMgr.add(pkg, res));
        pkgId = pkgMgr.getByName("TEST_AXIAL", res);
        ASSERT_GT(pkgId, 0);

        // Create fuse types
        FuseType fast("TEST_FAST");
        ASSERT_TRUE(typeMgr.add(fast, res));
        fastTypeId = typeMgr.getByName("TEST_FAST", res);
        ASSERT_GT(fastTypeId, 0);

        FuseType slow("TEST_SLOW");
        ASSERT_TRUE(typeMgr.add(slow, res));
        slowTypeId = typeMgr.getByName("TEST_SLOW", res);
        ASSERT_GT(slowTypeId, 0);
    }
};

// 1. AddFuse_InsertsRow
TEST_F(FuseManagerTest, AddFuse_InsertsRow) {
    Component c("TEST_FUSE", "Test Fuse", catId, manId, 10);
    ASSERT_TRUE(compMgr.add(c, res));
    ASSERT_GT(c.id, 0);

    Fuse f(c.id, pkgId, fastTypeId, 1.0, 250.0);
    ASSERT_TRUE(fuseMgr.add(f, res));

    Fuse fetched;
    ASSERT_TRUE(fuseMgr.getById(c.id, fetched, res));
    EXPECT_EQ(fetched.componentId, c.id);
    EXPECT_EQ(fetched.packageId, pkgId);
    EXPECT_EQ(fetched.typeId, fastTypeId);
}

// 2. GetFuseByComponentId_ReturnsCorrectFuse
TEST_F(FuseManagerTest, GetFuseByComponentId_ReturnsCorrectFuse) {
    Component c("TEST_FUSE_GET", "Test Fuse Get", catId, manId, 5);
    ASSERT_TRUE(compMgr.add(c, res));
    ASSERT_GT(c.id, 0);

    Fuse f(c.id, pkgId, fastTypeId, 0.5, 125.0);
    ASSERT_TRUE(fuseMgr.add(f, res));

    Fuse fetched;
    ASSERT_TRUE(fuseMgr.getById(c.id, fetched, res));
    EXPECT_EQ(fetched.componentId, c.id);
}

// 3. UpdateFuse_ChangesValues
TEST_F(FuseManagerTest, UpdateFuse_ChangesValues) {
    Component c("TEST_FUSE_UPDATE", "Test Fuse Update", catId, manId, 8);
    ASSERT_TRUE(compMgr.add(c, res));
    ASSERT_GT(c.id, 0);

    Fuse f(c.id, pkgId, fastTypeId, 1.0, 250.0);
    ASSERT_TRUE(fuseMgr.add(f, res));

    f.typeId = slowTypeId;
    f.currentRating = 0.25;
    f.voltageRating = 125.0;
    ASSERT_TRUE(fuseMgr.update(f, res));

    Fuse fetched;
    ASSERT_TRUE(fuseMgr.getById(c.id, fetched, res));
    EXPECT_EQ(fetched.typeId, slowTypeId);
}

// 4. DeleteFuse_RemovesRow
TEST_F(FuseManagerTest, DeleteFuse_RemovesRow) {
    Component c("TEST_FUSE_DELETE", "Test Fuse Delete", catId, manId, 3);
    ASSERT_TRUE(compMgr.add(c, res));
    ASSERT_GT(c.id, 0);

    Fuse f(c.id, pkgId, fastTypeId, 0.5, 125.0);
    ASSERT_TRUE(fuseMgr.add(f, res));

    ASSERT_TRUE(fuseMgr.remove(c.id, res));

    Fuse fetched;
    EXPECT_FALSE(fuseMgr.getById(c.id, fetched, res));
}

// 5. ListFuses_ReturnsAllFuses
TEST_F(FuseManagerTest, ListFuses_ReturnsAllFuses) {
    Component c1("TEST_FUSE_LIST1", "Test Fuse List 1", catId, manId, 10);
    ASSERT_TRUE(compMgr.add(c1, res));
    ASSERT_GT(c1.id, 0);

    Component c2("TEST_FUSE_LIST2", "Test Fuse List 2", catId, manId, 5);
    ASSERT_TRUE(compMgr.add(c2, res));
    ASSERT_GT(c2.id, 0);

    Fuse f1(c1.id, pkgId, fastTypeId, 1.0, 250.0);
    Fuse f2(c2.id, pkgId, slowTypeId, 0.25, 125.0);
    ASSERT_TRUE(fuseMgr.add(f1, res));
    ASSERT_TRUE(fuseMgr.add(f2, res));

    std::vector<Fuse> fuses;
    ASSERT_TRUE(fuseMgr.list(fuses, res));

    bool found1 = false, found2 = false;
    for (const auto& f : fuses) {
        if (f.componentId == c1.id) found1 = true;
        if (f.componentId == c2.id) found2 = true;
    }
    EXPECT_TRUE(found1);
    EXPECT_TRUE(found2);
}

// 6. AddFuse_WithInvalidComponent_Fails
TEST_F(FuseManagerTest, AddFuse_WithInvalidComponent_Fails) {
    Fuse f(999999, pkgId, fastTypeId, 1.0, 250.0);
    EXPECT_FALSE(fuseMgr.add(f, res));
}

// 7. GetFuseByComponentId_Nonexistent_ReturnsFalse
TEST_F(FuseManagerTest, GetFuseByComponentId_Nonexistent_ReturnsFalse) {
    Fuse fetched;
    EXPECT_FALSE(fuseMgr.getById(999999, fetched, res));
}
