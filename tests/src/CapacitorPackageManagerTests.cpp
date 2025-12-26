#include "BackendTestFixture.h"
#include "CapacitorPackageManager.h"

class CapacitorPackageManagerTest : public BackendTestFixture {
protected:
    CapacitorPackageManager pkgMgr;
    CapacitorPackageManagerTest() : pkgMgr(db) {}
};

// 1. Add_InsertsRow
TEST_F(CapacitorPackageManagerTest, Add_InsertsRow) {
    CapacitorPackage pkg("TestPkg_Add");
    ASSERT_TRUE(pkgMgr.add(pkg, res));

    int insertedId = pkgMgr.getByName("TestPkg_Add", res);
    ASSERT_GT(insertedId, 0);

    CapacitorPackage fetched;
    ASSERT_TRUE(pkgMgr.getById(insertedId, fetched, res));
    EXPECT_EQ(fetched.id, insertedId);
    EXPECT_EQ(fetched.name, "TestPkg_Add");
}

// 2. GetById_ReturnsCorrect
TEST_F(CapacitorPackageManagerTest, GetById_ReturnsCorrect) {
    CapacitorPackage pkg("TestPkg_Get");
    ASSERT_TRUE(pkgMgr.add(pkg, res));

    int insertedId = pkgMgr.getByName("TestPkg_Get", res);
    ASSERT_GT(insertedId, 0);

    CapacitorPackage fetched;
    ASSERT_TRUE(pkgMgr.getById(insertedId, fetched, res));
    EXPECT_EQ(fetched.id, insertedId);
    EXPECT_EQ(fetched.name, "TestPkg_Get");
}

// 3. List_ReturnsAll
TEST_F(CapacitorPackageManagerTest, List_ReturnsAll) {
    ASSERT_TRUE(pkgMgr.add(CapacitorPackage("TestPkg_List"), res));

    std::vector<CapacitorPackage> pkgs;
    ASSERT_TRUE(pkgMgr.list(pkgs, res));

    bool foundSeeded = false, foundTest = false;
    for (const auto& p : pkgs) {
        if (p.name == "Radial leaded") foundSeeded = true;
        if (p.name == "TestPkg_List") foundTest = true;
    }
    EXPECT_TRUE(foundSeeded);
    EXPECT_TRUE(foundTest);
}

// 4. Remove_RemovesRow
TEST_F(CapacitorPackageManagerTest, Remove_RemovesRow) {
    CapacitorPackage pkg("TestPkg_Delete");
    ASSERT_TRUE(pkgMgr.add(pkg, res));

    int insertedId = pkgMgr.getByName("TestPkg_Delete", res);
    ASSERT_GT(insertedId, 0);

    ASSERT_TRUE(pkgMgr.remove(insertedId, res));

    CapacitorPackage fetched;
    EXPECT_FALSE(pkgMgr.getById(insertedId, fetched, res));
}
