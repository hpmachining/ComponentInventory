#include "BackendTestFixture.h"
#include "ElectrolyticManager.h"
#include "ComponentManager.h"
#include "CapacitorManager.h"
#include "CapacitorPackageManager.h"
#include "CapacitorDielectricManager.h"

class ElectrolyticManagerTest : public BackendTestFixture {
protected:
    ElectrolyticManager elecMgr;
    ComponentManager compMgr;
    CapacitorManager capMgr;
    CapacitorPackageManager pkgMgr;
    CapacitorDielectricManager dielMgr;

    int pkgId;
    int dielId;
    int compId;

    ElectrolyticManagerTest()
        : elecMgr(db), compMgr(db), capMgr(db), pkgMgr(db), dielMgr(db),
        pkgId(0), dielId(0), compId(0) {
    }

    void SetUp() override {
        BackendTestFixture::SetUp(); // base fixture seeds lookups

        // Resolve seeded lookup IDs
        pkgId = pkgMgr.getByName("Radial leaded", res);
        ASSERT_GT(pkgId, 0);

        dielId = dielMgr.getByName("C0G/NP0", res);
        ASSERT_GT(dielId, 0);

        // Seed a component + capacitor base record
        Component comp("E-100", "Seed electrolytic component", catId, manId, 10, "Seed notes");
        ASSERT_TRUE(compMgr.addComponent(comp, res)) << res.toString();
        compId = compMgr.getByPartNumber("E-100", res);
        ASSERT_GT(compId, 0);

        Capacitor cap;
        cap.componentId = compId;
        cap.capacitance = 100e-6;
        cap.voltageRating = 25.0;
        cap.tolerance = 20.0;
        cap.esr = 0.5;
        cap.leakageCurrent = 5e-6;
        cap.polarized = true;
        cap.packageTypeId = pkgId;
        cap.dielectricTypeId = dielId;
        ASSERT_TRUE(capMgr.addCapacitor(cap, res)) << res.toString();
    }
};

// 1. AddElectrolytic_InsertsRow
TEST_F(ElectrolyticManagerTest, AddElectrolytic_InsertsRow) {
    Electrolytic e;
    e.componentId = compId;
    e.diameter = 6.3;
    e.height = 11.0;
    e.leadSpacing = 2.5;

    ASSERT_TRUE(elecMgr.addElectrolytic(e, res)) << res.toString();

    Electrolytic fetched;
    ASSERT_TRUE(elecMgr.getElectrolyticById(compId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.componentId, compId);
    EXPECT_DOUBLE_EQ(fetched.diameter, 6.3);
    EXPECT_DOUBLE_EQ(fetched.height, 11.0);
    EXPECT_DOUBLE_EQ(fetched.leadSpacing, 2.5);
}

// 2. GetElectrolyticById_ReturnsCorrectElectrolytic
TEST_F(ElectrolyticManagerTest, GetElectrolyticById_ReturnsCorrectElectrolytic) {
    Electrolytic e;
    e.componentId = compId;
    e.diameter = 10.0;
    e.height = 20.0;
    e.leadSpacing = 5.0;

    ASSERT_TRUE(elecMgr.addElectrolytic(e, res)) << res.toString();

    Electrolytic fetched;
    ASSERT_TRUE(elecMgr.getElectrolyticById(compId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.componentId, compId);
    EXPECT_DOUBLE_EQ(fetched.diameter, 10.0);
    EXPECT_DOUBLE_EQ(fetched.height, 20.0);
    EXPECT_DOUBLE_EQ(fetched.leadSpacing, 5.0);
}

// 3. UpdateElectrolytic_ChangesPersist
TEST_F(ElectrolyticManagerTest, UpdateElectrolytic_ChangesPersist) {
    Electrolytic e;
    e.componentId = compId;
    e.diameter = 8.0;
    e.height = 15.0;
    e.leadSpacing = 3.5;
    ASSERT_TRUE(elecMgr.addElectrolytic(e, res)) << res.toString();

    Electrolytic updated = e;
    updated.height = 18.0;
    updated.leadSpacing = 4.0;
    ASSERT_TRUE(elecMgr.updateElectrolytic(updated, res)) << res.toString();

    Electrolytic fetched;
    ASSERT_TRUE(elecMgr.getElectrolyticById(compId, fetched, res)) << res.toString();

    EXPECT_DOUBLE_EQ(fetched.height, 18.0);
    EXPECT_DOUBLE_EQ(fetched.leadSpacing, 4.0);
}

// 4. DeleteElectrolytic_RemovesRow
TEST_F(ElectrolyticManagerTest, DeleteElectrolytic_RemovesRow) {
    Electrolytic e;
    e.componentId = compId;
    e.diameter = 5.0;
    e.height = 12.0;
    e.leadSpacing = 2.0;
    ASSERT_TRUE(elecMgr.addElectrolytic(e, res)) << res.toString();

    ASSERT_TRUE(elecMgr.deleteElectrolytic(compId, res)) << res.toString();

    Electrolytic fetched;
    bool got = elecMgr.getElectrolyticById(compId, fetched, res);
    EXPECT_FALSE(got);  // should no longer exist
}

// 5. CascadeDelete_ComponentAlsoDeletesElectrolytic
TEST_F(ElectrolyticManagerTest, CascadeDelete_ComponentAlsoDeletesElectrolytic) {
    Electrolytic e;
    e.componentId = compId;
    e.diameter = 6.0;
    e.height = 13.0;
    e.leadSpacing = 2.5;
    ASSERT_TRUE(elecMgr.addElectrolytic(e, res)) << res.toString();

    // Delete the base component
    ASSERT_TRUE(compMgr.deleteComponent(compId, res)) << res.toString();

    // Electrolytic should be gone too
    Electrolytic fetched;
    bool got = elecMgr.getElectrolyticById(compId, fetched, res);
    EXPECT_FALSE(got);
}
