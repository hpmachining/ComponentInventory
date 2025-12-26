#include "BackendTestFixture.h"
#include "TransistorPackageManager.h"
#include <unordered_set>

// Fixture for TransistorPackageManager
class TransistorPackageManagerTest : public BackendTestFixture {
protected:
    TransistorPackageManager pkgMgr;

    TransistorPackageManagerTest()
        : pkgMgr(db) {
    }
};

// 1. Add_InsertsRow
TEST_F(TransistorPackageManagerTest, Add_InsertsRow) {
    TransistorPackage p("TestPackage");
    EXPECT_TRUE(pkgMgr.add(p, res));

    // Verify retrieval by name
    int id = pkgMgr.getByName("TestPackage", res);
    EXPECT_GT(id, 0);

    // Verify retrieval by ID
    TransistorPackage fetched;
    EXPECT_TRUE(pkgMgr.getById(id, fetched, res));
    EXPECT_EQ(fetched.name, "TestPackage");
}

// 2. Add_DuplicateFails
TEST_F(TransistorPackageManagerTest, Add_DuplicateFails) {
    TransistorPackage p("DuplicatePackage");
    ASSERT_TRUE(pkgMgr.add(p, res));
    // Second insert with same name should fail
    EXPECT_FALSE(pkgMgr.add(p, res));
}

// 3. Remove_RemovesRow
TEST_F(TransistorPackageManagerTest, Remove_RemovesRow) {
    TransistorPackage p("DeletePackage");
    ASSERT_TRUE(pkgMgr.add(p, res));
    int id = pkgMgr.getByName("DeletePackage", res);
    ASSERT_GT(id, 0);

    EXPECT_TRUE(pkgMgr.remove(id, res));

    // Verify it no longer exists
    TransistorPackage fetched;
    EXPECT_FALSE(pkgMgr.getById(id, fetched, res));
    EXPECT_EQ(pkgMgr.getByName("DeletePackage", res), -1);
}

// 4. List_ReturnsAll
TEST_F(TransistorPackageManagerTest, List_ReturnsAll) {
    TransistorPackage p1("Package1");
    TransistorPackage p2("Package2");
    ASSERT_TRUE(pkgMgr.add(p1, res));
    ASSERT_TRUE(pkgMgr.add(p2, res));

    std::vector<TransistorPackage> pkgs;
    EXPECT_TRUE(pkgMgr.list(pkgs, res));
    EXPECT_GE(pkgs.size(), 2u);

    std::unordered_set<std::string> names;
    for (const auto& p : pkgs) names.insert(p.name);

    EXPECT_NE(names.find("Package1"), names.end());
    EXPECT_NE(names.find("Package2"), names.end());
}

// 5. GetByName_NotFoundReturnsMinusOne
TEST_F(TransistorPackageManagerTest, GetByName_NotFoundReturnsMinusOne) {
    EXPECT_EQ(pkgMgr.getByName("NonExistentPackage", res), -1);
}

// 6. GetById_NotFoundReturnsFalse
TEST_F(TransistorPackageManagerTest, GetById_NotFoundReturnsFalse) {
    TransistorPackage p;
    EXPECT_FALSE(pkgMgr.getById(999999, p, res));
}
