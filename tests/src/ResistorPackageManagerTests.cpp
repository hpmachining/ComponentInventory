#include "BackendTestFixture.h"
#include "ResistorPackageManager.h"

class ResistorPackageManagerTest : public BackendTestFixture {
protected:
    ResistorPackageManager pkgMgr;
    ResistorPackageManagerTest() : pkgMgr(db) {}
};

// 1. AddPackage_InsertsRow
TEST_F(ResistorPackageManagerTest, AddPackage_InsertsRow) {
    ResistorPackage pkg("0805");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();

    int insertedPkgId = db.lastInsertId();
    ResistorPackage fetched;
    ASSERT_TRUE(pkgMgr.getPackageById(insertedPkgId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedPkgId);
    EXPECT_EQ(fetched.name, "0805");
}

// 2. GetPackageById_ReturnsCorrectPackage
TEST_F(ResistorPackageManagerTest, GetPackageById_ReturnsCorrectPackage) {
    ResistorPackage pkg("1206");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();
    int insertedPkgId = db.lastInsertId();

    ResistorPackage fetched;
    ASSERT_TRUE(pkgMgr.getPackageById(insertedPkgId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedPkgId);
    EXPECT_EQ(fetched.name, "1206");
}

// 3. ListPackages_ReturnsAllPackages
TEST_F(ResistorPackageManagerTest, ListPackages_ReturnsAllPackages) {
    ASSERT_TRUE(pkgMgr.addPackage(ResistorPackage("0603"), res)) << res.toString();
    ASSERT_TRUE(pkgMgr.addPackage(ResistorPackage("2512"), res)) << res.toString();

    std::vector<ResistorPackage> pkgs;
    ASSERT_TRUE(pkgMgr.listPackages(pkgs, res)) << res.toString();

    EXPECT_GE(pkgs.size(), 2);

    bool found0603 = false, found2512 = false;
    for (const auto& p : pkgs) {
        if (p.name == "0603") found0603 = true;
        if (p.name == "2512") found2512 = true;
    }
    EXPECT_TRUE(found0603);
    EXPECT_TRUE(found2512);
}

// 4. DeletePackage_RemovesRow
TEST_F(ResistorPackageManagerTest, DeletePackage_RemovesRow) {
    ResistorPackage pkg("DeleteMe");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();
    int insertedPkgId = db.lastInsertId();

    ASSERT_TRUE(pkgMgr.deletePackage(insertedPkgId, res)) << res.toString();

    ResistorPackage fetched;
    bool gotPkg = pkgMgr.getPackageById(insertedPkgId, fetched, res);
    EXPECT_FALSE(gotPkg);  // should not exist anymore
}
