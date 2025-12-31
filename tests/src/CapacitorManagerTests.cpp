#include "BackendTestFixture.h"
#include "CapacitorManager.h"
#include "ComponentManager.h"
#include "CapacitorPackageManager.h"
#include "CapacitorDielectricManager.h"

class CapacitorManagerTest : public BackendTestFixture {
protected:
    CapacitorManager capMgr;
    ComponentManager compMgr;
    CapacitorPackageManager pkgMgr;
    CapacitorDielectricManager dielMgr;

    int pkgId;
    int dielId;
    int compId;

    CapacitorManagerTest()
        : capMgr(db), compMgr(db), pkgMgr(db), dielMgr(db),
        pkgId(0), dielId(0), compId(0) {
    }

    void SetUp() override {
        BackendTestFixture::SetUp();

        pkgId = pkgMgr.getByName("Radial leaded", res);
        ASSERT_GT(pkgId, 0);

        dielId = dielMgr.getByName("C0G/NP0", res);
        ASSERT_GT(dielId, 0);

        Component comp("C-100nF", "Seed capacitor component", catId, manId, 50, "Seed notes");
        ASSERT_TRUE(compMgr.add(comp, res)) << res.toString();
        ASSERT_GT(comp.id, 0);
        compId = comp.id; // use id assigned by add()
    }
};

// 1. AddCapacitor_InsertsRow
TEST_F(CapacitorManagerTest, AddCapacitor_InsertsRow) {
    Capacitor c{ compId, 100e-9, 50.0, 5.0, 0.1, 0.001, false, pkgId, dielId };
    ASSERT_TRUE(capMgr.add(c, res)) << res.toString();

    Capacitor fetched;
    ASSERT_TRUE(capMgr.getById(compId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.componentId, compId);
    EXPECT_DOUBLE_EQ(fetched.capacitance, 100e-9);
    EXPECT_DOUBLE_EQ(fetched.voltageRating, 50.0);
    EXPECT_DOUBLE_EQ(fetched.tolerance, 5.0);
    EXPECT_DOUBLE_EQ(fetched.esr, 0.1);
    EXPECT_DOUBLE_EQ(fetched.leakageCurrent, 0.001);
    EXPECT_EQ(fetched.polarized, false);
    EXPECT_EQ(fetched.packageTypeId, pkgId);
    EXPECT_EQ(fetched.dielectricTypeId, dielId);
}

// 2. GetCapacitorById_ReturnsCorrect
TEST_F(CapacitorManagerTest, GetCapacitorById_ReturnsCorrect) {
    Capacitor c{ compId, 1e-6, 25.0, 10.0, 0.5, 0.01, true, pkgId, dielId };
    ASSERT_TRUE(capMgr.add(c, res)) << res.toString();

    Capacitor fetched;
    ASSERT_TRUE(capMgr.getById(compId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.componentId, compId);
    EXPECT_DOUBLE_EQ(fetched.capacitance, 1e-6);
    EXPECT_DOUBLE_EQ(fetched.voltageRating, 25.0);
    EXPECT_DOUBLE_EQ(fetched.tolerance, 10.0);
    EXPECT_DOUBLE_EQ(fetched.esr, 0.5);
    EXPECT_DOUBLE_EQ(fetched.leakageCurrent, 0.01);
    EXPECT_EQ(fetched.polarized, true);
}

// 3. UpdateCapacitor_ChangesPersist
TEST_F(CapacitorManagerTest, UpdateCapacitor_ChangesPersist) {
    Capacitor c{ compId, 10e-6, 25.0, 20.0, 0.5, 0.01, true, pkgId, dielId };
    ASSERT_TRUE(capMgr.add(c, res)) << res.toString();

    Capacitor updated = c;
    updated.tolerance = 15.0;
    updated.esr = 0.2;

    ASSERT_TRUE(capMgr.update(updated, res)) << res.toString();

    Capacitor fetched;
    ASSERT_TRUE(capMgr.getById(compId, fetched, res)) << res.toString();

    EXPECT_DOUBLE_EQ(fetched.tolerance, 15.0);
    EXPECT_DOUBLE_EQ(fetched.esr, 0.2);
}

// 4. RemoveCapacitor_DeletesRow
TEST_F(CapacitorManagerTest, RemoveCapacitor_DeletesRow) {
    Capacitor c{ compId, 1e-6, 16.0, 10.0, 0.3, 0.005, true, pkgId, dielId };
    ASSERT_TRUE(capMgr.add(c, res)) << res.toString();

    ASSERT_TRUE(capMgr.remove(compId, res)) << res.toString();

    Capacitor fetched;
    EXPECT_FALSE(capMgr.getById(compId, fetched, res));
}

// 5. ListCapacitors_ReturnsAll
TEST_F(CapacitorManagerTest, ListCapacitors_ReturnsAll) {
    Component c2("C-10uF", "Second capacitor", catId, manId, 10);
    ASSERT_TRUE(compMgr.add(c2, res)) << res.toString();
    int compId2 = c2.id;

    Capacitor c1{ compId, 100e-9, 50.0, 5.0, 0.1, 0.001, false, pkgId, dielId };
    Capacitor c2Obj{ compId2, 10e-6, 16.0, 10.0, 0.2, 0.002, true, pkgId, dielId };
    ASSERT_TRUE(capMgr.add(c1, res));
    ASSERT_TRUE(capMgr.add(c2Obj, res));

    std::vector<Capacitor> caps;
    ASSERT_TRUE(capMgr.list(caps, res));
    EXPECT_GE(caps.size(), 2);
}

// 6. ListLookup_ReturnsAllItems
//TEST_F(CapacitorManagerTest, ListLookup_ReturnsAllItems) {
//    std::vector<LookupItem> items;
//    ASSERT_TRUE(capMgr.listLookup(items, res));
//    EXPECT_FALSE(items.empty());
//}

// 7. ForeignKey_PackageAndDielectric_Valid
TEST_F(CapacitorManagerTest, ForeignKey_PackageAndDielectric_Valid) {
    Capacitor good{ compId, 1e-6, 16.0, 10.0, 0.3, 0.005, true, pkgId, dielId };
    EXPECT_TRUE(capMgr.add(good, res));

    Capacitor badPkg{ compId, 1e-6, 16.0, 10.0, 0.3, 0.005, true, 999999, dielId };
    EXPECT_FALSE(capMgr.add(badPkg, res));

    Capacitor badDiel{ compId, 1e-6, 16.0, 10.0, 0.3, 0.005, true, pkgId, 999999 };
    EXPECT_FALSE(capMgr.add(badDiel, res));
}
