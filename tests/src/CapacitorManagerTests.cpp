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

        // Seed a package
        CapacitorPackage pkg("Radial");
        ASSERT_TRUE(pkgMgr.addPackage(pkg, res)) << res.toString();
        pkgId = db.lastInsertId();

        // Seed a dielectric
        CapacitorDielectric diel("Ceramic");
        ASSERT_TRUE(dielMgr.addDielectric(diel, res)) << res.toString();
        dielId = db.lastInsertId();

        // Seed a component
        Component comp("C-100", "Seed capacitor component", catId, manId, 50, "Seed notes");
        ASSERT_TRUE(compMgr.addComponent(comp, res)) << res.toString();
        compId = db.lastInsertId();
    }
};

// 1. AddCapacitor_InsertsRow
TEST_F(CapacitorManagerTest, AddCapacitor_InsertsRow) {
    Capacitor c;
    c.componentId = compId;
    c.capacitance = 1e-6; // 1 µF
    c.voltageRating = 50.0;
    c.tolerance = 10.0;
    c.esr = 0.1;
    c.leakageCurrent = 1e-6;
    c.polarized = false;
    c.packageTypeId = pkgId;
    c.dielectricTypeId = dielId;

    ASSERT_TRUE(capMgr.addCapacitor(c, res)) << res.toString();

    Capacitor fetched;
    ASSERT_TRUE(capMgr.getCapacitorById(compId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.componentId, compId);
    EXPECT_DOUBLE_EQ(fetched.capacitance, 1e-6);
    EXPECT_DOUBLE_EQ(fetched.voltageRating, 50.0);
    EXPECT_DOUBLE_EQ(fetched.tolerance, 10.0);
    EXPECT_DOUBLE_EQ(fetched.esr, 0.1);
    EXPECT_DOUBLE_EQ(fetched.leakageCurrent, 1e-6);
    EXPECT_FALSE(fetched.polarized);
    EXPECT_EQ(fetched.packageTypeId, pkgId);
    EXPECT_EQ(fetched.dielectricTypeId, dielId);
}

// 2. GetCapacitorById_ReturnsCorrectCapacitor
TEST_F(CapacitorManagerTest, GetCapacitorById_ReturnsCorrectCapacitor) {
    Capacitor c;
    c.componentId = compId;
    c.capacitance = 4.7e-6;
    c.voltageRating = 25.0;
    c.tolerance = 20.0;
    c.esr = 0.2;
    c.leakageCurrent = 2e-6;
    c.polarized = true;
    c.packageTypeId = pkgId;
    c.dielectricTypeId = dielId;

    ASSERT_TRUE(capMgr.addCapacitor(c, res)) << res.toString();

    Capacitor fetched;
    ASSERT_TRUE(capMgr.getCapacitorById(compId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.componentId, compId);
    EXPECT_DOUBLE_EQ(fetched.capacitance, 4.7e-6);
    EXPECT_DOUBLE_EQ(fetched.voltageRating, 25.0);
    EXPECT_DOUBLE_EQ(fetched.tolerance, 20.0);
    EXPECT_DOUBLE_EQ(fetched.esr, 0.2);
    EXPECT_DOUBLE_EQ(fetched.leakageCurrent, 2e-6);
    EXPECT_TRUE(fetched.polarized);
}

// 3. UpdateCapacitor_ChangesPersist
TEST_F(CapacitorManagerTest, UpdateCapacitor_ChangesPersist) {
    Capacitor c;
    c.componentId = compId;
    c.capacitance = 10e-6;
    c.voltageRating = 16.0;
    c.tolerance = 5.0;
    c.esr = 0.05;
    c.leakageCurrent = 5e-6;
    c.polarized = true;
    c.packageTypeId = pkgId;
    c.dielectricTypeId = dielId;

    ASSERT_TRUE(capMgr.addCapacitor(c, res)) << res.toString();

    Capacitor updated = c;
    updated.tolerance = 2.0;
    updated.voltageRating = 25.0;
    updated.esr = 0.01;

    ASSERT_TRUE(capMgr.updateCapacitor(updated, res)) << res.toString();

    Capacitor fetched;
    ASSERT_TRUE(capMgr.getCapacitorById(compId, fetched, res)) << res.toString();

    EXPECT_DOUBLE_EQ(fetched.tolerance, 2.0);
    EXPECT_DOUBLE_EQ(fetched.voltageRating, 25.0);
    EXPECT_DOUBLE_EQ(fetched.esr, 0.01);
}

// 4. DeleteCapacitor_RemovesRow
TEST_F(CapacitorManagerTest, DeleteCapacitor_RemovesRow) {
    Capacitor c;
    c.componentId = compId;
    c.capacitance = 2.2e-6;
    c.voltageRating = 50.0;
    c.tolerance = 10.0;
    c.esr = 0.1;
    c.leakageCurrent = 1e-6;
    c.polarized = false;
    c.packageTypeId = pkgId;
    c.dielectricTypeId = dielId;

    ASSERT_TRUE(capMgr.addCapacitor(c, res)) << res.toString();

    ASSERT_TRUE(capMgr.deleteCapacitor(compId, res)) << res.toString();

    Capacitor fetched;
    bool got = capMgr.getCapacitorById(compId, fetched, res);
    EXPECT_FALSE(got);  // should no longer exist
}

// 5. ForeignKey_ComponentPackageDielectric_Valid
TEST_F(CapacitorManagerTest, ForeignKey_ComponentPackageDielectric_Valid) {
    Capacitor valid;
    valid.componentId = compId;
    valid.capacitance = 1e-6;
    valid.voltageRating = 50.0;
    valid.tolerance = 5.0;
    valid.esr = 0.1;
    valid.leakageCurrent = 1e-6;
    valid.polarized = false;
    valid.packageTypeId = pkgId;
    valid.dielectricTypeId = dielId;
    ASSERT_TRUE(capMgr.addCapacitor(valid, res)) << res.toString();

    Capacitor badComp = valid;
    badComp.componentId = 999999;
    EXPECT_FALSE(capMgr.addCapacitor(badComp, res));

    Capacitor badPkg = valid;
    badPkg.packageTypeId = 999999;
    EXPECT_FALSE(capMgr.addCapacitor(badPkg, res));

    Capacitor badDiel = valid;
    badDiel.dielectricTypeId = 999999;
    EXPECT_FALSE(capMgr.addCapacitor(badDiel, res));
}
