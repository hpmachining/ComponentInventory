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
    ResistorPackage pkg("TestPkg_Add");
    ASSERT_TRUE(pkgMgr.add(pkg, res)) << res.toString();

    int id = pkgMgr.getByName("TestPkg_Add", res);
    ASSERT_GT(id, 0);

    ResistorPackage fetched;
    ASSERT_TRUE(pkgMgr.getById(id, fetched, res)) << res.toString();
    EXPECT_EQ(fetched.id, id);
    EXPECT_EQ(fetched.name, "TestPkg_Add");
}

// 2. GetPackageById_ReturnsCorrectPackage
TEST_F(ResistorPackageManagerTest, GetPackageById_ReturnsCorrectPackage) {
    ResistorPackage pkg("TestPkg_Get");
    ASSERT_TRUE(pkgMgr.add(pkg, res)) << res.toString();

    int id = pkgMgr.getByName("TestPkg_Get", res);
    ASSERT_GT(id, 0);

    ResistorPackage fetched;
    ASSERT_TRUE(pkgMgr.getById(id, fetched, res)) << res.toString();
    EXPECT_EQ(fetched.name, "TestPkg_Get");
}

// 3. ListPackages_ReturnsAllPackages
TEST_F(ResistorPackageManagerTest, ListPackages_ReturnsAllPackages) {
    ASSERT_TRUE(pkgMgr.add(ResistorPackage("TestPkg_List"), res)) << res.toString();

    std::vector<ResistorPackage> pkgs;
    ASSERT_TRUE(pkgMgr.list(pkgs, res)) << res.toString();
    ASSERT_GE(pkgs.size(), 1);

    bool foundSeeded = false, foundTest = false;
    for (const auto& p : pkgs) {
        if (p.name == "Carbon Film") foundSeeded = true;       // example seeded package
        if (p.name == "TestPkg_List") foundTest = true;
    }
    EXPECT_TRUE(foundTest);
    // We can optionally expect seeded ones if they exist
    // EXPECT_TRUE(foundSeeded);
}

// 4. UpdatePackage_ChangesValues
TEST_F(ResistorPackageManagerTest, UpdatePackage_ChangesValues) {
    ResistorPackage pkg("TestPkg_Update");
    ASSERT_TRUE(pkgMgr.add(pkg, res)) << res.toString();

    int id = pkgMgr.getByName("TestPkg_Update", res);
    ASSERT_GT(id, 0);

    ResistorPackage toUpdate;
    ASSERT_TRUE(pkgMgr.getById(id, toUpdate, res));

    toUpdate.name = "TestPkg_Updated";
    ASSERT_TRUE(pkgMgr.update(toUpdate, res)) << res.toString();

    ResistorPackage fetched;
    ASSERT_TRUE(pkgMgr.getById(id, fetched, res)) << res.toString();
    EXPECT_EQ(fetched.name, "TestPkg_Updated");
}

// 5. DeletePackage_RemovesRow
TEST_F(ResistorPackageManagerTest, DeletePackage_RemovesRow) {
    ResistorPackage pkg("TestPkg_Delete");
    ASSERT_TRUE(pkgMgr.add(pkg, res)) << res.toString();

    int id = pkgMgr.getByName("TestPkg_Delete", res);
    ASSERT_GT(id, 0);

    ASSERT_TRUE(pkgMgr.remove(id, res)) << res.toString();

    ResistorPackage fetched;
    EXPECT_FALSE(pkgMgr.getById(id, fetched, res));
}

// 6. AddPackage_WithDuplicateName_Fails
TEST_F(ResistorPackageManagerTest, AddPackage_WithDuplicateName_Fails) {
    ResistorPackage pkg("TestPkg_Duplicate");
    ASSERT_TRUE(pkgMgr.add(pkg, res)) << res.toString();

    // Adding again should fail if UNIQUE constraint exists
    ResistorPackage dup("TestPkg_Duplicate");
    EXPECT_FALSE(pkgMgr.add(dup, res));
}

// 7. GetPackageById_Nonexistent_ReturnsFalse
TEST_F(ResistorPackageManagerTest, GetPackageById_Nonexistent_ReturnsFalse) {
    ResistorPackage fetched;
    EXPECT_FALSE(pkgMgr.getById(999999, fetched, res));
}
