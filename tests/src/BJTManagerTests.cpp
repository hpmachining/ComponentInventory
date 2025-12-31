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
        : bjtMgr(db),
        transistorMgr(db),
        compMgr(db),
        typeMgr(db),
        polMgr(db),
        pkgMgr(db) {
    }
};

// Helper: create base transistor setup
static int setupTransistor(ComponentManager& compMgr,
    TransistorManager& transistorMgr,
    TransistorTypeManager& typeMgr,
    TransistorPolarityManager& polMgr,
    TransistorPackageManager& pkgMgr,
    DbResult& res,
    const Component& cTemplate)
{
    // Make a modifiable copy so add() can set c.id
    Component c = cTemplate;
    if (!compMgr.add(c, res)) {
        ADD_FAILURE() << "Failed to add component: " << res.toString();
        return -1;
    }

    if (c.id <= 0) {
        ADD_FAILURE() << "add() succeeded but returned invalid id";
        return -1;
    }

    int typeId = typeMgr.getByName("BJT", res);
    if (typeId <= 0) {
        ADD_FAILURE() << "Could not find transistor type BJT";
        return -1;
    }

    int polId = polMgr.getByName("NPN", res);
    if (polId <= 0) {
        ADD_FAILURE() << "Could not find transistor polarity NPN";
        return -1;
    }

    int pkgId = pkgMgr.getByName("TO-92", res);
    if (pkgId <= 0) {
        ADD_FAILURE() << "Could not find transistor package TO-92";
        return -1;
    }

    if (!transistorMgr.add(Transistor(c.id, typeId, polId, pkgId), res)) {
        ADD_FAILURE() << "Failed to add Transistor record: " << res.toString();
        return -1;
    }

    return c.id;
}

// 1. AddBJT_InsertsRow
TEST_F(BJTManagerTest, AddBJT_InsertsRow) {
    int compId = setupTransistor(
        compMgr, transistorMgr, typeMgr, polMgr, pkgMgr, res,
        Component("PN2222", "Unit test transistor", catId, manId, 1));

    BJT b(compId, 40.0, 0.2, 0.5, 200.0, 100e6);
    EXPECT_TRUE(bjtMgr.add(b, res));

    BJT fetched;
    EXPECT_TRUE(bjtMgr.getById(compId, fetched, res));
    EXPECT_EQ(fetched.hfe, 200.0);
}

// 2. UpdateBJT_ChangesPersist
TEST_F(BJTManagerTest, UpdateBJT_ChangesPersist) {
    int compId = setupTransistor(
        compMgr, transistorMgr, typeMgr, polMgr, pkgMgr, res,
        Component("PN2907", "Unit test transistor", catId, manId, 1));

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
    int compId = setupTransistor(
        compMgr, transistorMgr, typeMgr, polMgr, pkgMgr, res,
        Component("PN3904", "Unit test transistor", catId, manId, 1));

    ASSERT_TRUE(bjtMgr.add(
        BJT(compId, 40.0, 0.2, 0.5, 100.0, 50e6), res));

    EXPECT_TRUE(bjtMgr.remove(compId, res));

    BJT fetched;
    EXPECT_FALSE(bjtMgr.getById(compId, fetched, res));
}

// 4. ListBJTs_ReturnsAll
TEST_F(BJTManagerTest, ListBJTs_ReturnsAll) {
    int compId1 = setupTransistor(
        compMgr, transistorMgr, typeMgr, polMgr, pkgMgr, res,
        Component("PN3906", "Unit test transistor", catId, manId, 1));

    ASSERT_TRUE(bjtMgr.add(
        BJT(compId1, 40.0, 0.2, 0.5, 100.0, 50e6), res));

    int compId2 = setupTransistor(
        compMgr, transistorMgr, typeMgr, polMgr, pkgMgr, res,
        Component("2N3055", "Unit test transistor", catId, manId, 1));

    ASSERT_TRUE(bjtMgr.add(
        BJT(compId2, 60.0, 0.5, 1.0, 200.0, 80e6), res));

    std::vector<BJT> bjts;
    EXPECT_TRUE(bjtMgr.list(bjts, res));
    EXPECT_EQ(bjts.size(), 2);
}

// 5. ListLookup_ReturnsAllItems
TEST_F(BJTManagerTest, ListLookup_ReturnsAllItems) {
    int compId1 = setupTransistor(
        compMgr, transistorMgr, typeMgr, polMgr, pkgMgr, res,
        Component("PN3906", "Unit test transistor", catId, manId, 1));

    ASSERT_TRUE(bjtMgr.add(
        BJT(compId1, 40.0, 0.2, 0.5, 100.0, 50e6), res));

    int compId2 = setupTransistor(
        compMgr, transistorMgr, typeMgr, polMgr, pkgMgr, res,
        Component("2N3055", "Unit test transistor", catId, manId, 1));

    ASSERT_TRUE(bjtMgr.add(
        BJT(compId2, 60.0, 0.5, 1.0, 200.0, 80e6), res));

    std::vector<LookupItem> items;
    EXPECT_TRUE(bjtMgr.listLookup(items, res));

    EXPECT_EQ(items.size(), 2);

    bool found1 = false, found2 = false;
    for (const auto& item : items) {
        if (item.id == compId1 && item.name == "PN3906") found1 = true;
        if (item.id == compId2 && item.name == "2N3055") found2 = true;
    }

    EXPECT_TRUE(found1);
    EXPECT_TRUE(found2);
}
