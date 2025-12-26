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
        BackendTestFixture::SetUp(); // base fixture seeds lookups

        pkgId = pkgMgr.getByName("Radial leaded", res);
        ASSERT_GT(pkgId, 0);

        dielId = dielMgr.getByName("C0G/NP0", res);
        ASSERT_GT(dielId, 0);

        Component comp("C-100nF", "Seed capacitor component", catId, manId, 50, "Seed notes");
        ASSERT_TRUE(compMgr.addComponent(comp, res)) << res.toString();
        compId = compMgr.getByPartNumber("C-100nF", res);
        ASSERT_GT(compId, 0);
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

// 2. UpdateCapacitor_ChangesPersist
TEST_F(CapacitorManagerTest, UpdateCapacitor_ChangesPersist) {
    Capacitor c{ compId, 10e-6, 25.0, 20.0, 0.5, 0.01, true, pkgId, dielId };
    ASSERT_TRUE(capMgr.add(c, res)) << res.toString();

    Capacitor updated = c;
    updated.tolerance = 10.0;
    updated.esr = 0.2;

    ASSERT_TRUE(capMgr.update(updated, res)) << res.toString();

    Capacitor fetched;
    ASSERT_TRUE(capMgr.getById(compId, fetched, res)) << res.toString();
    EXPECT_DOUBLE_EQ(fetched.tolerance, 10.0);
    EXPECT_DOUBLE_EQ(fetched.esr, 0.2);
}

// 3. RemoveCapacitor_DeletesRow
TEST_F(CapacitorManagerTest, RemoveCapacitor_DeletesRow) {
    Capacitor c{ compId, 1e-6, 16.0, 10.0, 0.3, 0.005, true, pkgId, dielId };
    ASSERT_TRUE(capMgr.add(c, res)) << res.toString();

    ASSERT_TRUE(capMgr.remove(compId, res)) << res.toString();

    Capacitor fetched;
    EXPECT_FALSE(capMgr.getById(compId, fetched, res));
}
