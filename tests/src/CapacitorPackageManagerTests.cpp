#include "BackendTestFixture.h"
#include "CapacitorPackageManager.h"

class CapacitorPackageManagerTest : public BackendTestFixture {
protected:
    CapacitorPackageManager pkgMgr;
    CapacitorPackageManagerTest() : pkgMgr(db) {}
};

// 1. AddPackage_InsertsRow
TEST_F(CapacitorPackageManagerTest, AddPackage_InsertsRow) {
    CapacitorPackage pkg("Radial");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();

    int insertedPkgId = db.lastInsertId();
    CapacitorPackage fetched;
    ASSERT_TRUE(pkgMgr.getPackageById(insertedPkgId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedPkgId);
    EXPECT_EQ(fetched.name, "Radial");
}

// 2. GetPackageById_ReturnsCorrectPackage
TEST_F(CapacitorPackageManagerTest, GetPackageById_ReturnsCorrectPackage) {
    CapacitorPackage pkg("Axial");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();
    int insertedPkgId = db.lastInsertId();

    CapacitorPackage fetched;
    ASSERT_TRUE(pkgMgr.getPackageById(insertedPkgId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedPkgId);
    EXPECT_EQ(fetched.name, "Axial");
}

// 3. ListPackages_ReturnsAllPackages
TEST_F(CapacitorPackageManagerTest, ListPackages_ReturnsAllPackages) {
    ASSERT_TRUE(pkgMgr.addPackage(CapacitorPackage("SMD"), res)) << res.toString();
    ASSERT_TRUE(pkgMgr.addPackage(CapacitorPackage("Through-hole"), res)) << res.toString();

    std::vector<CapacitorPackage> pkgs;
    ASSERT_TRUE(pkgMgr.listPackages(pkgs, res)) << res.toString();

    EXPECT_GE(pkgs.size(), 2);

    bool foundSMD = false, foundTH = false;
    for (const auto& p : pkgs) {
        if (p.name == "SMD") foundSMD = true;
        if (p.name == "Through-hole") foundTH = true;
    }
    EXPECT_TRUE(foundSMD);
    EXPECT_TRUE(foundTH);
}

// 4. DeletePackage_RemovesRow
TEST_F(CapacitorPackageManagerTest, DeletePackage_RemovesRow) {
    CapacitorPackage pkg("DeleteMe");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();
    int insertedPkgId = db.lastInsertId();

    ASSERT_TRUE(pkgMgr.deletePackage(insertedPkgId, res)) << res.toString();

    CapacitorPackage fetched;
    bool gotPkg = pkgMgr.getPackageById(insertedPkgId, fetched, res);
    EXPECT_FALSE(gotPkg);  // should not exist anymore
}
