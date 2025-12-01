#include "BackendTestFixture.h"
#include "ResistorPackageManager.h"

class ResistorPackageManagerTest : public BackendTestFixture {
protected:
    ResistorPackageManager pkgMgr;

    ResistorPackageManagerTest()
        : pkgMgr(db) {
    }
};

// 1. AddPackage_InsertsRow
TEST_F(ResistorPackageManagerTest, AddPackage_InsertsRow) {
    // Use a unique test-only name to avoid collision with seeded packages
    ResistorPackage pkg("TestPkg_Add");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();

    int id = pkgMgr.getByName("TestPkg_Add", res);
    ASSERT_GT(id, 0);

    ResistorPackage fetched;
    ASSERT_TRUE(pkgMgr.getPackageById(id, fetched, res)) << res.toString();
    EXPECT_EQ(fetched.id, id);
    EXPECT_EQ(fetched.name, "TestPkg_Add");
}

// 2. GetPackageById_ReturnsCorrectPackage
TEST_F(ResistorPackageManagerTest, GetPackageById_ReturnsCorrectPackage) {
    ResistorPackage pkg("TestPkg_Get");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();

    int id = pkgMgr.getByName("TestPkg_Get", res);
    ASSERT_GT(id, 0);

    ResistorPackage fetched;
    ASSERT_TRUE(pkgMgr.getPackageById(id, fetched, res)) << res.toString();
    EXPECT_EQ(fetched.name, "TestPkg_Get");
}

// 3. ListPackages_ReturnsAllPackages
TEST_F(ResistorPackageManagerTest, ListPackages_ReturnsAllPackages) {
    // Add a unique package
    ASSERT_TRUE(pkgMgr.addPackage(ResistorPackage("TestPkg_List"), res)) << res.toString();

    std::vector<ResistorPackage> pkgs;
    ASSERT_TRUE(pkgMgr.listPackages(pkgs, res)) << res.toString();

    bool has0805 = false, hasTestPkg = false;
    for (const auto& p : pkgs) {
        if (p.name == "0805") has0805 = true;          // seeded canonical
        if (p.name == "TestPkg_List") hasTestPkg = true; // test-only
    }
    EXPECT_TRUE(has0805);
    EXPECT_TRUE(hasTestPkg);
}

// 4. DeletePackage_RemovesRow
TEST_F(ResistorPackageManagerTest, DeletePackage_RemovesRow) {
    ResistorPackage pkg("TestPkg_Delete");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();

    int id = pkgMgr.getByName("TestPkg_Delete", res);
    ASSERT_GT(id, 0);

    ASSERT_TRUE(pkgMgr.deletePackage(id, res)) << res.toString();

    ResistorPackage fetched;
    EXPECT_FALSE(pkgMgr.getPackageById(id, fetched, res));
}
