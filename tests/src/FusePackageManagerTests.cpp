#include "BackendTestFixture.h"
#include "FusePackageManager.h"

class FusePackageManagerTest : public BackendTestFixture {
protected:
    FusePackageManager pkgMgr;

    FusePackageManagerTest()
        : pkgMgr(db) {
    }
};

// 1. AddFusePackage_InsertsRow
TEST_F(FusePackageManagerTest, AddFusePackage_InsertsRow) {
    FusePackage pkg("TestFusePkg_Add");
    ASSERT_TRUE(pkgMgr.add(pkg, res)) << res.toString();

    int id = pkgMgr.getByName("TestFusePkg_Add", res);
    ASSERT_GT(id, 0);

    FusePackage fetched;
    ASSERT_TRUE(pkgMgr.getById(id, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, id);
    EXPECT_EQ(fetched.name, "TestFusePkg_Add");
}

// 2. GetFusePackageById_ReturnsCorrectPackage
TEST_F(FusePackageManagerTest, GetFusePackageById_ReturnsCorrectPackage) {
    ASSERT_TRUE(pkgMgr.add(FusePackage("TestFusePkg_Get"), res)) << res.toString();

    int id = pkgMgr.getByName("TestFusePkg_Get", res);
    ASSERT_GT(id, 0);

    FusePackage fetched;
    ASSERT_TRUE(pkgMgr.getById(id, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, id);
    EXPECT_EQ(fetched.name, "TestFusePkg_Get");
}

// 3. ListFusePackages_ReturnsAllPackages
TEST_F(FusePackageManagerTest, ListFusePackages_ReturnsAllPackages) {
    const std::string addedName = "TestFusePkg_List";
    ASSERT_TRUE(pkgMgr.add(FusePackage(addedName), res)) << res.toString();

    std::vector<FusePackage> pkgs;
    ASSERT_TRUE(pkgMgr.list(pkgs, res)) << res.toString();

    bool foundTest = false;
    for (const auto& p : pkgs) {
        if (p.name == addedName) {
            foundTest = true;
            break;
        }
    }

    EXPECT_FALSE(pkgs.empty());
    EXPECT_TRUE(foundTest);
}

// 4. RemoveFusePackage_DeletesRow
TEST_F(FusePackageManagerTest, RemoveFusePackage_DeletesRow) {
    ASSERT_TRUE(pkgMgr.add(FusePackage("TestFusePkg_Delete"), res)) << res.toString();

    int id = pkgMgr.getByName("TestFusePkg_Delete", res);
    ASSERT_GT(id, 0);

    ASSERT_TRUE(pkgMgr.remove(id, res)) << res.toString();

    FusePackage fetched;
    EXPECT_FALSE(pkgMgr.getById(id, fetched, res));
}

// 5. GetFusePackageByName_ReturnsId
TEST_F(FusePackageManagerTest, GetFusePackageByName_ReturnsId) {
    ASSERT_TRUE(pkgMgr.add(FusePackage("TestFusePkg_ByName"), res)) << res.toString();

    int id = pkgMgr.getByName("TestFusePkg_ByName", res);
    EXPECT_GT(id, 0);
}

// 6. AddFusePackage_DuplicateNameFails
TEST_F(FusePackageManagerTest, AddFusePackage_DuplicateNameFails) {
    ASSERT_TRUE(pkgMgr.add(FusePackage("TestFusePkg_Dup"), res)) << res.toString();

    DbResult dupRes;
    EXPECT_FALSE(pkgMgr.add(FusePackage("TestFusePkg_Dup"), dupRes));
    EXPECT_TRUE(dupRes.hasError());
}

// 7. GetFusePackageById_Nonexistent
TEST_F(FusePackageManagerTest, GetFusePackageById_NonexistentReturnsFalse) {
    FusePackage fetched;
    EXPECT_FALSE(pkgMgr.getById(9999, fetched, res));
}
