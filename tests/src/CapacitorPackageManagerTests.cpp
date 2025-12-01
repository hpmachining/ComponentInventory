#include "BackendTestFixture.h"
#include "CapacitorPackageManager.h"

class CapacitorPackageManagerTest : public BackendTestFixture {
protected:
    CapacitorPackageManager pkgMgr;
    CapacitorPackageManagerTest() : pkgMgr(db) {}
};

// 1. AddPackage_InsertsRow
TEST_F(CapacitorPackageManagerTest, AddPackage_InsertsRow) {
    // Use a unique test-only name to avoid collision with seeded packages
    CapacitorPackage pkg("TestCapPkg_Add");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();

    int insertedPkgId = pkgMgr.getByName("TestCapPkg_Add", res);
    ASSERT_GT(insertedPkgId, 0);

    CapacitorPackage fetched;
    ASSERT_TRUE(pkgMgr.getPackageById(insertedPkgId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedPkgId);
    EXPECT_EQ(fetched.name, "TestCapPkg_Add");
}

// 2. GetPackageById_ReturnsCorrectPackage
TEST_F(CapacitorPackageManagerTest, GetPackageById_ReturnsCorrectPackage) {
    CapacitorPackage pkg("TestCapPkg_Get");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();

    int insertedPkgId = pkgMgr.getByName("TestCapPkg_Get", res);
    ASSERT_GT(insertedPkgId, 0);

    CapacitorPackage fetched;
    ASSERT_TRUE(pkgMgr.getPackageById(insertedPkgId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedPkgId);
    EXPECT_EQ(fetched.name, "TestCapPkg_Get");
}

// 3. ListPackages_ReturnsAllPackages
TEST_F(CapacitorPackageManagerTest, ListPackages_ReturnsAllPackages) {
    // Add a unique package
    ASSERT_TRUE(pkgMgr.addPackage(CapacitorPackage("TestCapPkg_List"), res)) << res.toString();

    std::vector<CapacitorPackage> pkgs;
    ASSERT_TRUE(pkgMgr.listPackages(pkgs, res)) << res.toString();

    bool foundRadial = false, foundTestPkg = false;
    for (const auto& p : pkgs) {
        if (p.name == "Radial leaded") foundRadial = true;        // seeded canonical
        if (p.name == "TestCapPkg_List") foundTestPkg = true;     // test-only
    }
    EXPECT_TRUE(foundRadial);
    EXPECT_TRUE(foundTestPkg);
}

// 4. DeletePackage_RemovesRow
TEST_F(CapacitorPackageManagerTest, DeletePackage_RemovesRow) {
    CapacitorPackage pkg("TestCapPkg_Delete");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();

    int insertedPkgId = pkgMgr.getByName("TestCapPkg_Delete", res);
    ASSERT_GT(insertedPkgId, 0);

    ASSERT_TRUE(pkgMgr.deletePackage(insertedPkgId, res)) << res.toString();

    CapacitorPackage fetched;
    bool gotPkg = pkgMgr.getPackageById(insertedPkgId, fetched, res);
    EXPECT_FALSE(gotPkg);  // should not exist anymore
}
