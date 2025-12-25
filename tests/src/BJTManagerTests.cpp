#include "BackendTestFixture.h"
#include "BJTManager.h"
#include "TransistorManager.h"
#include "ComponentManager.h"
#include "TransistorTypeManager.h"
#include "TransistorPolarityManager.h"
#include "TransistorPackageManager.h"

// Derived fixture for BJT tests
class BJTManagerTest : public BackendTestFixture {
protected:
    BJTManager bjtMgr;
    TransistorManager transistorMgr;
    ComponentManager compMgr;
    TransistorTypeManager typeMgr;
    TransistorPolarityManager polMgr;
    TransistorPackageManager pkgMgr;

    BJTManagerTest()
        : bjtMgr(db), transistorMgr(db), compMgr(db),
        typeMgr(db), polMgr(db), pkgMgr(db) {
    }
};

// 1. AddBJT_InsertsRow
TEST_F(BJTManagerTest, AddBJT_InsertsRow) {
    Component c("PN2222", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = compMgr.getByPartNumber("PN2222", res);

    int typeId = typeMgr.getByName("BJT", res);
    ASSERT_GT(typeId, 0);
    int polId = polMgr.getByName("NPN", res);
    ASSERT_GT(polId, 0);
    int pkgId = pkgMgr.getByName("TO-92", res);
    ASSERT_GT(pkgId, 0);

    Transistor t(compId, typeId, polId, pkgId);
    ASSERT_TRUE(transistorMgr.addTransistor(t, res));

    BJT b(compId, 40.0, 0.2, 0.5, 200.0, 100e6);
    EXPECT_TRUE(bjtMgr.add(b, res));

    BJT fetched;
    EXPECT_TRUE(bjtMgr.getById(compId, fetched, res));
    EXPECT_EQ(fetched.hfe, 200.0);
}

// 2. UpdateBJT_ChangesPersist
TEST_F(BJTManagerTest, UpdateBJT_ChangesPersist) {
    Component c("PN2907", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = compMgr.getByPartNumber("PN2907", res);

    int typeId = typeMgr.getByName("BJT", res);
    ASSERT_GT(typeId, 0);
    int polId = polMgr.getByName("NPN", res);
    ASSERT_GT(polId, 0);
    int pkgId = pkgMgr.getByName("TO-92", res);
    ASSERT_GT(pkgId, 0);

    Transistor t(compId, typeId, polId, pkgId);
    ASSERT_TRUE(transistorMgr.addTransistor(t, res));

    BJT b(compId, 40.0, 0.2, 0.5, 150.0, 80e6);
    ASSERT_TRUE(bjtMgr.add(b, res));

    b.hfe = 300.0;
    EXPECT_TRUE(bjtMgr.update(b, res));

    BJT fetched;
    EXPECT_TRUE(bjtMgr.getById(compId, fetched, res));
    EXPECT_EQ(fetched.hfe, 300.0);
}

// 3. DeleteBJT_RemovesRow
TEST_F(BJTManagerTest, DeleteBJT_RemovesRow) {
    Component c("PN3904", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = compMgr.getByPartNumber("PN3904", res);

    int typeId = typeMgr.getByName("BJT", res);
    ASSERT_GT(typeId, 0);
    int polId = polMgr.getByName("NPN", res);
    ASSERT_GT(polId, 0);
    int pkgId = pkgMgr.getByName("TO-92", res);
    ASSERT_GT(pkgId, 0);

    Transistor t(compId, typeId, polId, pkgId);
    ASSERT_TRUE(transistorMgr.addTransistor(t, res));

    BJT b(compId, 40.0, 0.2, 0.5, 100.0, 50e6);
    ASSERT_TRUE(bjtMgr.add(b, res));

    EXPECT_TRUE(bjtMgr.remove(compId, res));

    BJT fetched;
    EXPECT_FALSE(bjtMgr.getById(compId, fetched, res));
}

// 4. ListBJTs_ReturnsAll
TEST_F(BJTManagerTest, ListBJTs_ReturnsAll) {
    Component c1("PN3906", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c1, res));
    int compId1 = compMgr.getByPartNumber("PN3906", res);

    int typeId = typeMgr.getByName("BJT", res);
    ASSERT_GT(typeId, 0);
    int polId1 = polMgr.getByName("NPN", res);
    ASSERT_GT(polId1, 0);
    int pkgId = pkgMgr.getByName("TO-92", res);
    ASSERT_GT(pkgId, 0);

    ASSERT_TRUE(transistorMgr.addTransistor(Transistor(compId1, typeId, polId1, pkgId), res));
    ASSERT_TRUE(bjtMgr.add(BJT(compId1, 40.0, 0.2, 0.5, 100.0, 50e6), res));

    Component c2("2N3055", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c2, res));
    int compId2 = compMgr.getByPartNumber("2N3055", res);

    int polId2 = polMgr.getByName("NPN", res);
    ASSERT_GT(polId2, 0);

    ASSERT_TRUE(transistorMgr.addTransistor(Transistor(compId2, typeId, polId2, pkgId), res));
    ASSERT_TRUE(bjtMgr.add(BJT(compId2, 60.0, 0.5, 1.0, 200.0, 80e6), res));

    std::vector<BJT> bjts;
    EXPECT_TRUE(bjtMgr.list(bjts, res));
    EXPECT_EQ(bjts.size(), 2);
}

// 5. ListLookup_ReturnsAllItems
TEST_F(BJTManagerTest, ListLookup_ReturnsAllItems) {
    // --- Component 1 ---
    Component c1("PN3906", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c1, res));
    int compId1 = compMgr.getByPartNumber("PN3906", res);

    int typeId = typeMgr.getByName("BJT", res);
    ASSERT_GT(typeId, 0);
    int polId1 = polMgr.getByName("NPN", res);
    ASSERT_GT(polId1, 0);
    int pkgId = pkgMgr.getByName("TO-92", res);
    ASSERT_GT(pkgId, 0);

    ASSERT_TRUE(transistorMgr.addTransistor(Transistor(compId1, typeId, polId1, pkgId), res));
    ASSERT_TRUE(bjtMgr.add(BJT(compId1, 40.0, 0.2, 0.5, 100.0, 50e6), res));

    // --- Component 2 ---
    Component c2("2N3055", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c2, res));
    int compId2 = compMgr.getByPartNumber("2N3055", res);

    int polId2 = polMgr.getByName("NPN", res);
    ASSERT_GT(polId2, 0);

    ASSERT_TRUE(transistorMgr.addTransistor(Transistor(compId2, typeId, polId2, pkgId), res));
    ASSERT_TRUE(bjtMgr.add(BJT(compId2, 60.0, 0.5, 1.0, 200.0, 80e6), res));

    // --- Call listLookup ---
    std::vector<LookupItem> items;
    EXPECT_TRUE(bjtMgr.listLookup(items, res));

    // --- Validate ---
    EXPECT_EQ(items.size(), 2);
    bool found1 = false, found2 = false;
    for (const auto& item : items) {
        if (item.id == compId1 && item.name == "PN3906") found1 = true;
        if (item.id == compId2 && item.name == "2N3055") found2 = true;
    }
    EXPECT_TRUE(found1);
    EXPECT_TRUE(found2);
}
