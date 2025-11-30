#include "BackendTestFixture.h"
#include "ComponentManager.h"
#include <thread>
#include <chrono>

class ComponentManagerTest : public BackendTestFixture {
protected:
    ComponentManager compMgr;
    ComponentManagerTest() : compMgr(db) {}
};

// 1. AddComponent_InsertsRow (tests all fields)
TEST_F(ComponentManagerTest, AddComponent_InsertsRow) {
    Component comp("PN123", "Test component", catId, manId, 10, "Notes");
    ASSERT_TRUE(compMgr.addComponent(comp, res)) << res.toString();

    int insertedCompId = db.lastInsertId();
    Component fetched;
    ASSERT_TRUE(compMgr.getComponentById(insertedCompId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedCompId);
    EXPECT_EQ(fetched.partNumber, "PN123");
    EXPECT_EQ(fetched.description, "Test component");
    EXPECT_EQ(fetched.categoryId, catId);
    EXPECT_EQ(fetched.manufacturerId, manId);
    EXPECT_EQ(fetched.quantity, 10);
    EXPECT_EQ(fetched.notes, "Notes");

    // CreatedOn and ModifiedOn should be non-empty timestamps
    EXPECT_FALSE(fetched.createdOn.empty());
    EXPECT_FALSE(fetched.modifiedOn.empty());
}

// 2. GetComponentById_ReturnsCorrectComponent
TEST_F(ComponentManagerTest, GetComponentById_ReturnsCorrectComponent) {
    Component comp("PN456", "Another test component", catId, manId, 5, "More notes");
    ASSERT_TRUE(compMgr.addComponent(comp, res)) << res.toString();

    int insertedCompId = db.lastInsertId();

    Component fetched;
    ASSERT_TRUE(compMgr.getComponentById(insertedCompId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedCompId);
    EXPECT_EQ(fetched.partNumber, "PN456");
    EXPECT_EQ(fetched.description, "Another test component");
    EXPECT_EQ(fetched.categoryId, catId);
    EXPECT_EQ(fetched.manufacturerId, manId);
    EXPECT_EQ(fetched.quantity, 5);
    EXPECT_EQ(fetched.notes, "More notes");
    EXPECT_FALSE(fetched.createdOn.empty());
    EXPECT_FALSE(fetched.modifiedOn.empty());
}

// 3. UpdateComponent_ChangesPersist (tests ModifiedOn changes)
TEST_F(ComponentManagerTest, UpdateComponent_ChangesPersist) {
    Component comp("PN789", "To be updated", catId, manId, 1, "Old notes");
    ASSERT_TRUE(compMgr.addComponent(comp, res)) << res.toString();

    int insertedCompId = db.lastInsertId();

    Component updated("PN789-REV1", "Updated description", catId, manId, 3, "New notes");
    updated.id = insertedCompId;

    // Pause to ensure CreatedOn and ModifiedOn differ (SQLite datetime has 1s resolution)
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_TRUE(compMgr.updateComponent(updated, res)) << res.toString();

    Component fetched;
    ASSERT_TRUE(compMgr.getComponentById(insertedCompId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedCompId);
    EXPECT_EQ(fetched.partNumber, "PN789-REV1");
    EXPECT_EQ(fetched.description, "Updated description");
    EXPECT_EQ(fetched.quantity, 3);
    EXPECT_EQ(fetched.notes, "New notes");

    // CreatedOn should remain the same, ModifiedOn should update
    EXPECT_FALSE(fetched.createdOn.empty());
    EXPECT_FALSE(fetched.modifiedOn.empty());
    EXPECT_NE(fetched.createdOn, fetched.modifiedOn);
}

// 4. DeleteComponent_RemovesRow
TEST_F(ComponentManagerTest, DeleteComponent_RemovesRow) {
    Component comp("PNDEL", "Delete me", catId, manId, 2, "Temp");
    ASSERT_TRUE(compMgr.addComponent(comp, res)) << res.toString();

    int insertedCompId = db.lastInsertId();

    ASSERT_TRUE(compMgr.deleteComponent(insertedCompId, res)) << res.toString();

    Component fetched;
    bool got = compMgr.getComponentById(insertedCompId, fetched, res);
    EXPECT_FALSE(got);  // should no longer exist
}

// 5. ForeignKey_CategoryAndManufacturer_Valid
TEST_F(ComponentManagerTest, ForeignKey_CategoryAndManufacturer_Valid) {
    // Valid insert with seeded foreign keys
    Component valid("PNFK1", "FK control", catId, manId, 1, "");
    ASSERT_TRUE(compMgr.addComponent(valid, res)) << res.toString();

    // Invalid category
    Component badCat("PNFK2", "Bad category", /*categoryId*/ 999999, manId, 1, "");
    bool okBadCat = compMgr.addComponent(badCat, res);
    EXPECT_FALSE(okBadCat);

    // Invalid manufacturer
    Component badMan("PNFK3", "Bad manufacturer", catId, /*manId*/ 999999, 1, "");
    bool okBadMan = compMgr.addComponent(badMan, res);
    EXPECT_FALSE(okBadMan);
}
