#include "BackendTestFixture.h"
#include "TransistorManager.h"
#include "ComponentManager.h"

// Derived fixture for Transistor tests
class TransistorManagerTest : public BackendTestFixture {
protected:
    TransistorManager transistorMgr;
    ComponentManager compMgr;

    TransistorManagerTest()
        : transistorMgr(db), compMgr(db) {
    }
};

// 1. AddTransistor_InsertsRow
TEST_F(TransistorManagerTest, AddTransistor_InsertsRow) {
    // Seed a component
    Component c("PN1000", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = db.lastInsertId();

    // Insert into Transistors
    Transistor t(compId, 1, 1, 1);
    EXPECT_TRUE(transistorMgr.addTransistor(t, res));

    // Verify retrieval
    Transistor fetched;
    EXPECT_TRUE(transistorMgr.getTransistorById(compId, fetched, res));
    EXPECT_EQ(fetched.componentId, compId);
    EXPECT_EQ(fetched.typeId, 1);
    EXPECT_EQ(fetched.polarityId, 1);
    EXPECT_EQ(fetched.packageId, 1);
}

// 2. DeleteTransistor_RemovesRow
TEST_F(TransistorManagerTest, DeleteTransistor_RemovesRow) {
    Component c("PN1001", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = db.lastInsertId();

    Transistor t(compId, 1, 1, 1);
    ASSERT_TRUE(transistorMgr.addTransistor(t, res));

    EXPECT_TRUE(transistorMgr.deleteTransistor(compId, res));

    Transistor fetched;
    EXPECT_FALSE(transistorMgr.getTransistorById(compId, fetched, res));
}

// 3. ListTransistors_ReturnsAll
TEST_F(TransistorManagerTest, ListTransistors_ReturnsAll) {
    // First component
    Component c1("PN1002", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c1, res));
    int compId1 = db.lastInsertId();
    ASSERT_TRUE(transistorMgr.addTransistor(Transistor(compId1, 1, 1, 1), res));

    // Second component
    Component c2("PN1003", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c2, res));
    int compId2 = db.lastInsertId();
    ASSERT_TRUE(transistorMgr.addTransistor(Transistor(compId2, 1, 1, 1), res));

    // Verify list
    std::vector<Transistor> ts;
    EXPECT_TRUE(transistorMgr.listTransistors(ts, res));
    EXPECT_EQ(ts.size(), 2);
}
