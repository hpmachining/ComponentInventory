#include "BackendTestFixture.h"
#include "DiodePackageManager.h"

class DiodePackageManagerTest : public BackendTestFixture {
protected:
    DiodePackageManager pkgMgr;

    DiodePackageManagerTest()
        : pkgMgr(db) {
    }
};

// 1. AddDiodePackage_InsertsRow
TEST_F(DiodePackageManagerTest, AddDiodePackage_InsertsRow) {
    DiodePackage pkg("TestDiodePkg_Add");
    ASSERT_TRUE(pkgMgr.add(pkg, res)) << res.toString();

    int id = pkgMgr.getByName("TestDiodePkg_Add", res);
    ASSERT_GT(id, 0);

    DiodePackage fetched;
    ASSERT_TRUE(pkgMgr.getById(id, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, id);
    EXPECT_EQ(fetched.name, "TestDiodePkg_Add");
}

// 2. GetDiodePackageById_ReturnsCorrectPackage
TEST_F(DiodePackageManagerTest, GetDiodePackageById_ReturnsCorrectPackage) {
    ASSERT_TRUE(pkgMgr.add(DiodePackage("TestDiodePkg_Get"), res)) << res.toString();

    int id = pkgMgr.getByName("TestDiodePkg_Get", res);
    ASSERT_GT(id, 0);

    DiodePackage fetched;
    ASSERT_TRUE(pkgMgr.getById(id, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, id);
    EXPECT_EQ(fetched.name, "TestDiodePkg_Get");
}

// 3. ListDiodePackages_ReturnsAllPackages
TEST_F(DiodePackageManagerTest, ListDiodePackages_ReturnsAllPackages) {
    const std::string addedName = "TestDiodePkg_List";
    ASSERT_TRUE(pkgMgr.add(DiodePackage(addedName), res)) << res.toString();

    std::vector<DiodePackage> pkgs;
    ASSERT_TRUE(pkgMgr.list(pkgs, res)) << res.toString();

    // list should contain at least the added package
    bool foundTest = false;
    for (const auto& p : pkgs) {
        if (p.name == addedName) { foundTest = true; break; }
    }

    EXPECT_FALSE(pkgs.empty());
    EXPECT_TRUE(foundTest);
}

// 4. RemoveDiodePackage_DeletesRow
TEST_F(DiodePackageManagerTest, RemoveDiodePackage_DeletesRow) {
    ASSERT_TRUE(pkgMgr.add(DiodePackage("TestDiodePkg_Delete"), res)) << res.toString();

    int id = pkgMgr.getByName("TestDiodePkg_Delete", res);
    ASSERT_GT(id, 0);

    ASSERT_TRUE(pkgMgr.remove(id, res)) << res.toString();

    DiodePackage fetched;
    EXPECT_FALSE(pkgMgr.getById(id, fetched, res));
}
