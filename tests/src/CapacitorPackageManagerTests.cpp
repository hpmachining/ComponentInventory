#include "BackendTestFixture.h"
#include "CapacitorPackageManager.h"

class CapacitorPackageManagerTest : public BackendTestFixture {
protected:
    CapacitorPackageManager pkgMgr;
    CapacitorPackageManagerTest() : pkgMgr(db) {}
};

// 1. AddPackage_InsertsRow
TEST_F(CapacitorPackageManagerTest, AddPackage_InsertsRow) {
    // Use a distinct name to avoid collision with fixture baseline
    CapacitorPackage pkg("Axial");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();

    int insertedPkgId = pkgMgr.getByName("Axial", res);
    ASSERT_GT(insertedPkgId, 0);

    CapacitorPackage fetched;
    ASSERT_TRUE(pkgMgr.getPackageById(insertedPkgId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedPkgId);
    EXPECT_EQ(fetched.name, "Axial");
}

// 2. GetPackageById_ReturnsCorrectPackage
TEST_F(CapacitorPackageManagerTest, GetPackageById_ReturnsCorrectPackage) {
    CapacitorPackage pkg("SMD");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();

    int insertedPkgId = pkgMgr.getByName("SMD", res);
    ASSERT_GT(insertedPkgId, 0);

    CapacitorPackage fetched;
    ASSERT_TRUE(pkgMgr.getPackageById(insertedPkgId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedPkgId);
    EXPECT_EQ(fetched.name, "SMD");
}

// 3. ListPackages_ReturnsAllPackages
TEST_F(CapacitorPackageManagerTest, ListPackages_ReturnsAllPackages) {
    ASSERT_TRUE(pkgMgr.addPackage(CapacitorPackage("Through-hole"), res)) << res.toString();
    ASSERT_TRUE(pkgMgr.addPackage(CapacitorPackage("Chip"), res)) << res.toString();

    std::vector<CapacitorPackage> pkgs;
    ASSERT_TRUE(pkgMgr.listPackages(pkgs, res)) << res.toString();

    EXPECT_GE(pkgs.size(), 2);

    bool foundTH = false, foundChip = false;
    for (const auto& p : pkgs) {
        if (p.name == "Through-hole") foundTH = true;
        if (p.name == "Chip") foundChip = true;
    }
    EXPECT_TRUE(foundTH);
    EXPECT_TRUE(foundChip);
}

// 4. DeletePackage_RemovesRow
TEST_F(CapacitorPackageManagerTest, DeletePackage_RemovesRow) {
    CapacitorPackage pkg("DeleteMe");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();

    int insertedPkgId = pkgMgr.getByName("DeleteMe", res);
    ASSERT_GT(insertedPkgId, 0);

    ASSERT_TRUE(pkgMgr.deletePackage(insertedPkgId, res)) << res.toString();

    CapacitorPackage fetched;
    bool gotPkg = pkgMgr.getPackageById(insertedPkgId, fetched, res);
    EXPECT_FALSE(gotPkg);  // should not exist anymore
}
