#include "BackendTestFixture.h"
#include "BJTManager.h"
#include "TransistorManager.h"
#include "ComponentManager.h"

// Derived fixture for BJT tests
class BJTManagerTest : public BackendTestFixture {
protected:
    BJTManager bjtMgr;
    TransistorManager transistorMgr;
    ComponentManager compMgr;

    BJTManagerTest()
        : bjtMgr(db), transistorMgr(db), compMgr(db) {
    }
};

// 1. AddBJT_InsertsRow
TEST_F(BJTManagerTest, AddBJT_InsertsRow) {
    // Seed a component
    Component c("PN2222", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = db.lastInsertId();

    // Insert into Transistors
    Transistor t(compId, 1, 1, 1);
    ASSERT_TRUE(transistorMgr.addTransistor(t, res));

    // Insert into BJTs
    BJT b(compId, 40.0, 0.2, 0.5, 200.0, 100e6);
    EXPECT_TRUE(bjtMgr.addBJT(b, res));

    // Verify
    BJT fetched;
    EXPECT_TRUE(bjtMgr.getBJTById(compId, fetched, res));
    EXPECT_EQ(fetched.hfe, 200.0);
}

// 2. UpdateBJT_ChangesPersist
TEST_F(BJTManagerTest, UpdateBJT_ChangesPersist) {
    Component c("PN2907", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = db.lastInsertId();

    Transistor t(compId, 1, 1, 1);
    ASSERT_TRUE(transistorMgr.addTransistor(t, res));

    BJT b(compId, 40.0, 0.2, 0.5, 150.0, 80e6);
    ASSERT_TRUE(bjtMgr.addBJT(b, res));

    // Update gain
    b.hfe = 300.0;
    EXPECT_TRUE(bjtMgr.updateBJT(b, res));

    BJT fetched;
    EXPECT_TRUE(bjtMgr.getBJTById(compId, fetched, res));
    EXPECT_EQ(fetched.hfe, 300.0);
}

// 3. DeleteBJT_RemovesRow
TEST_F(BJTManagerTest, DeleteBJT_RemovesRow) {
    Component c("PN3904", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = db.lastInsertId();

    Transistor t(compId, 1, 1, 1);
    ASSERT_TRUE(transistorMgr.addTransistor(t, res));

    BJT b(compId, 40.0, 0.2, 0.5, 100.0, 50e6);
    ASSERT_TRUE(bjtMgr.addBJT(b, res));

    EXPECT_TRUE(bjtMgr.deleteBJT(compId, res));

    BJT fetched;
    EXPECT_FALSE(bjtMgr.getBJTById(compId, fetched, res));
}

// 4. ListBJTs_ReturnsAll
TEST_F(BJTManagerTest, ListBJTs_ReturnsAll) {
    // First component
    Component c1("PN3906", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c1, res));
    int compId1 = db.lastInsertId();

    Transistor t1(compId1, 1, 1, 1);
    ASSERT_TRUE(transistorMgr.addTransistor(t1, res));
    BJT b1(compId1, 40.0, 0.2, 0.5, 100.0, 50e6);
    ASSERT_TRUE(bjtMgr.addBJT(b1, res));

    // Second component
    Component c2("2N3055", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c2, res));
    int compId2 = db.lastInsertId();

    Transistor t2(compId2, 1, 1, 1);
    ASSERT_TRUE(transistorMgr.addTransistor(t2, res));
    BJT b2(compId2, 60.0, 0.5, 1.0, 200.0, 80e6);
    ASSERT_TRUE(bjtMgr.addBJT(b2, res));

    // Verify list
    std::vector<BJT> bjts;
    EXPECT_TRUE(bjtMgr.listBJTs(bjts, res));
    EXPECT_EQ(bjts.size(), 2);
}
