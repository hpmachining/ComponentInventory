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
    // Use a distinct name to avoid collision with fixture's "0805"
    ResistorPackage pkg("0603");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();

    int id = pkgMgr.getByName("0603", res);
    ASSERT_GT(id, 0);

    ResistorPackage fetched;
    ASSERT_TRUE(pkgMgr.getPackageById(id, fetched, res)) << res.toString();
    EXPECT_EQ(fetched.id, id);
    EXPECT_EQ(fetched.name, "0603");
}

// 2. GetPackageById_ReturnsCorrectPackage
TEST_F(ResistorPackageManagerTest, GetPackageById_ReturnsCorrectPackage) {
    ResistorPackage pkg("1206");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();

    int id = pkgMgr.getByName("1206", res);
    ASSERT_GT(id, 0);

    ResistorPackage fetched;
    ASSERT_TRUE(pkgMgr.getPackageById(id, fetched, res)) << res.toString();
    EXPECT_EQ(fetched.name, "1206");
}

// 3. ListPackages_ReturnsAllPackages
TEST_F(ResistorPackageManagerTest, ListPackages_ReturnsAllPackages) {
    ResistorPackage pkg("2512");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();

    std::vector<ResistorPackage> pkgs;
    ASSERT_TRUE(pkgMgr.listPackages(pkgs, res)) << res.toString();
    ASSERT_GE(pkgs.size(), 2);

    bool has0805 = false, has2512 = false;
    for (const auto& p : pkgs) {
        if (p.name == "0805") has0805 = true;
        if (p.name == "2512") has2512 = true;
    }
    EXPECT_TRUE(has0805);
    EXPECT_TRUE(has2512);
}

// 4. DeletePackage_RemovesRow
TEST_F(ResistorPackageManagerTest, DeletePackage_RemovesRow) {
    ResistorPackage pkg("2010");
    ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();

    int id = pkgMgr.getByName("2010", res);
    ASSERT_GT(id, 0);

    ASSERT_TRUE(pkgMgr.deletePackage(id, res)) << res.toString();

    ResistorPackage fetched;
    EXPECT_FALSE(pkgMgr.getPackageById(id, fetched, res));
}
