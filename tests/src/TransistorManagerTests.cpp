#include "BackendTestFixture.h"
#include "TransistorManager.h"
#include "ComponentManager.h"
#include "TransistorTypeManager.h"
#include "TransistorPolarityManager.h"
#include "TransistorPackageManager.h"
#include <unordered_set>

// Derived fixture for Transistor tests
class TransistorManagerTest : public BackendTestFixture {
protected:
    TransistorManager transistorMgr;
    ComponentManager compMgr;
    TransistorTypeManager typeMgr;
    TransistorPolarityManager polMgr;
    TransistorPackageManager pkgMgr;

    TransistorManagerTest()
        : transistorMgr(db), compMgr(db),
        typeMgr(db), polMgr(db), pkgMgr(db) {
    }
};

// 1. AddTransistor_InsertsRow
TEST_F(TransistorManagerTest, AddTransistor_InsertsRow) {
    // Seed a component
    Component c("PN1000", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.add(c, res));

    // Resolve lookup IDs by name (seeded in BackendTestFixture)
    int typeId = typeMgr.getByName("BJT", res);
    ASSERT_GT(typeId, 0);
    int polId = polMgr.getByName("NPN", res);
    ASSERT_GT(polId, 0);
    int pkgId = pkgMgr.getByName("TO-92", res);
    ASSERT_GT(pkgId, 0);

    // Insert into Transistors
    Transistor t(c.id, typeId, polId, pkgId);
    EXPECT_TRUE(transistorMgr.add(t, res));

    // Verify retrieval
    Transistor fetched;
    EXPECT_TRUE(transistorMgr.getById(c.id, fetched, res));
    EXPECT_EQ(fetched.componentId, c.id);
    EXPECT_EQ(fetched.typeId, typeId);
    EXPECT_EQ(fetched.polarityId, polId);
    EXPECT_EQ(fetched.packageId, pkgId);
}

// 2. DeleteTransistor_RemovesRow
TEST_F(TransistorManagerTest, DeleteTransistor_RemovesRow) {
    Component c("PN1001", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.add(c, res));

    int typeId = typeMgr.getByName("BJT", res);
    int polId = polMgr.getByName("NPN", res);
    int pkgId = pkgMgr.getByName("TO-92", res);

    Transistor t(c.id, typeId, polId, pkgId);
    ASSERT_TRUE(transistorMgr.add(t, res));

    EXPECT_TRUE(transistorMgr.remove(c.id, res));

    Transistor fetched;
    EXPECT_FALSE(transistorMgr.getById(c.id, fetched, res));
}

// 3. ListTransistors_ReturnsAll
TEST_F(TransistorManagerTest, ListTransistors_ReturnsAll) {
    // First component
    Component c1("PN1002", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.add(c1, res));

    int typeId = typeMgr.getByName("BJT", res);
    int polId = polMgr.getByName("NPN", res);
    int pkgId = pkgMgr.getByName("TO-92", res);

    ASSERT_TRUE(transistorMgr.add(Transistor(c1.id, typeId, polId, pkgId), res));

    // Second component
    Component c2("PN1003", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.add(c2, res));

    ASSERT_TRUE(transistorMgr.add(Transistor(c2.id, typeId, polId, pkgId), res));

    // Verify listx`xxxx
    std::vector<Transistor> ts;
	ts.clear();
    EXPECT_TRUE(transistorMgr.list(ts, res));
    EXPECT_EQ(ts.size(), 2u);

    // ensure the two expected component IDs are present in the result
    std::unordered_set<int> ids;
    for (const auto& t : ts) {
        ids.insert(t.componentId);
    }
    EXPECT_EQ(ids.size(), 2u); // sanity check
    EXPECT_NE(ids.find(c1.id), ids.end());
    EXPECT_NE(ids.find(c2.id), ids.end());
}

// 4. AddTransistor_DuplicateFails
TEST_F(TransistorManagerTest, AddTransistor_DuplicateFails) {
    Component c("PN1004", "Unit test transistor", catId, manId, 1);
    ASSERT_TRUE(compMgr.add(c, res));

    int typeId = typeMgr.getByName("BJT", res);
    int polId = polMgr.getByName("NPN", res);
    int pkgId = pkgMgr.getByName("TO-92", res);

    Transistor t(c.id, typeId, polId, pkgId);

    ASSERT_TRUE(transistorMgr.add(t, res));
    EXPECT_FALSE(transistorMgr.add(t, res));  // duplicate
}

// 5. AddTransistor_InvalidComponentFails
TEST_F(TransistorManagerTest, AddTransistor_InvalidComponentFails) {
    int typeId = typeMgr.getByName("BJT", res);
    int polId = polMgr.getByName("NPN", res);
    int pkgId = pkgMgr.getByName("TO-92", res);

    // componentId does not exist
    Transistor t(99999, typeId, polId, pkgId);
    EXPECT_FALSE(transistorMgr.add(t, res));
}

// 6. GetById_NotFoundReturnsFalse
TEST_F(TransistorManagerTest, GetById_NotFoundReturnsFalse) {
    Transistor t;
    EXPECT_FALSE(transistorMgr.getById(123456, t, res));
}
