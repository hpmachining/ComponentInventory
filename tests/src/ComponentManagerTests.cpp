#include "BackendTestFixture.h"
#include "ComponentManager.h"
#include <thread>
#include <chrono>

class ComponentManagerTest : public BackendTestFixture {
protected:
    ComponentManager compMgr;
    ComponentManagerTest() : compMgr(db) {}
};

// 1. Add_InsertsRow (tests all fields + ID assignment)
TEST_F(ComponentManagerTest, Add_InsertsRow) {
    Component comp("PN123", "Test component", catId, manId, 10, "Notes");
    ASSERT_TRUE(compMgr.add(comp, res)) << res.toString();
    ASSERT_GT(comp.id, 0);

    Component fetched;
    ASSERT_TRUE(compMgr.getById(comp.id, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, comp.id);
    EXPECT_EQ(fetched.partNumber, "PN123");
    EXPECT_EQ(fetched.description, "Test component");
    EXPECT_EQ(fetched.categoryId, catId);
    EXPECT_EQ(fetched.manufacturerId, manId);
    EXPECT_EQ(fetched.quantity, 10);
    EXPECT_EQ(fetched.notes, "Notes");

    EXPECT_FALSE(fetched.createdOn.empty());
    EXPECT_FALSE(fetched.modifiedOn.empty());
}

// 2. GetById_ReturnsCorrectComponent
TEST_F(ComponentManagerTest, GetById_ReturnsCorrectComponent) {
    Component comp("PN456", "Another test component", catId, manId, 5, "More notes");
    ASSERT_TRUE(compMgr.add(comp, res)) << res.toString();
    ASSERT_GT(comp.id, 0);

    Component fetched;
    ASSERT_TRUE(compMgr.getById(comp.id, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, comp.id);
    EXPECT_EQ(fetched.partNumber, "PN456");
    EXPECT_EQ(fetched.description, "Another test component");
    EXPECT_EQ(fetched.categoryId, catId);
    EXPECT_EQ(fetched.manufacturerId, manId);
    EXPECT_EQ(fetched.quantity, 5);
    EXPECT_EQ(fetched.notes, "More notes");
}

// 3. Update_ChangesPersist (tests ModifiedOn changes)
TEST_F(ComponentManagerTest, Update_ChangesPersist) {
    Component comp("PN789", "To be updated", catId, manId, 1, "Old notes");
    ASSERT_TRUE(compMgr.add(comp, res)) << res.toString();
    ASSERT_GT(comp.id, 0);

    Component updated = comp;
    updated.partNumber = "PN789-REV1";
    updated.description = "Updated description";
    updated.quantity = 3;
    updated.notes = "New notes";

    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_TRUE(compMgr.update(updated, res)) << res.toString();

    Component fetched;
    ASSERT_TRUE(compMgr.getById(comp.id, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.partNumber, "PN789-REV1");
    EXPECT_EQ(fetched.description, "Updated description");
    EXPECT_EQ(fetched.quantity, 3);
    EXPECT_EQ(fetched.notes, "New notes");
    EXPECT_NE(fetched.createdOn, fetched.modifiedOn);
}

// 4. Remove_DeletesRow
TEST_F(ComponentManagerTest, Remove_DeletesRow) {
    Component comp("PNDEL", "Delete me", catId, manId, 2, "Temp");
    ASSERT_TRUE(compMgr.add(comp, res)) << res.toString();
    ASSERT_GT(comp.id, 0);

    ASSERT_TRUE(compMgr.remove(comp.id, res)) << res.toString();

    Component fetched;
    EXPECT_FALSE(compMgr.getById(comp.id, fetched, res));
}

// 5. ForeignKey_CategoryAndManufacturer_Enforced
TEST_F(ComponentManagerTest, ForeignKey_CategoryAndManufacturer_Enforced) {
    Component valid("PNFK1", "FK control", catId, manId, 1, "");
    ASSERT_TRUE(compMgr.add(valid, res)) << res.toString();

    Component badCat("PNFK2", "Bad category", 999999, manId, 1, "");
    EXPECT_FALSE(compMgr.add(badCat, res));

    Component badMan("PNFK3", "Bad manufacturer", catId, 999999, 1, "");
    EXPECT_FALSE(compMgr.add(badMan, res));
}

// 6. List_ReturnsAllInsertedComponents
TEST_F(ComponentManagerTest, List_ReturnsAllInsertedComponents) {
    Component c1("PNL1", "List test 1", catId, manId, 1);
    Component c2("PNL2", "List test 2", catId, manId, 2);

    ASSERT_TRUE(compMgr.add(c1, res));
    ASSERT_TRUE(compMgr.add(c2, res));

    std::vector<Component> comps;
    ASSERT_TRUE(compMgr.list(comps, res)) << res.toString();

    EXPECT_GE(comps.size(), 2u);
}

// 7. Remove_CascadesToSubtables (schema-level behavior)
TEST_F(ComponentManagerTest, Remove_CascadesToSubtables) {
    Component comp("PNCAS", "Cascade test", catId, manId, 1);
    ASSERT_TRUE(compMgr.add(comp, res)) << res.toString();
    ASSERT_GT(comp.id, 0);

    // Insert a resistor row directly to verify cascade behavior
    ASSERT_TRUE(db.exec(
        "INSERT INTO Resistors (ComponentID, Resistance) VALUES (" +
        std::to_string(comp.id) + ", 1000);",
        res));

    ASSERT_TRUE(compMgr.remove(comp.id, res)) << res.toString();

    EXPECT_EQ(db.countRows("Resistors",
        "ComponentID=" + std::to_string(comp.id)), 0);
}
