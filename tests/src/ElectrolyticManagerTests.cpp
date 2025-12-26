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
        : elecMgr(db)
        , compMgr(db)
        , capMgr(db)
        , pkgMgr(db)
        , dielMgr(db)
        , pkgId(0)
        , dielId(0)
        , compId(0) {
    }

    void SetUp() override {
        BackendTestFixture::SetUp(); // seeds lookups

        pkgId = pkgMgr.getByName("Radial leaded", res);
        ASSERT_GT(pkgId, 0);

        dielId = dielMgr.getByName("C0G/NP0", res);
        ASSERT_GT(dielId, 0);

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
        ASSERT_TRUE(capMgr.add(cap, res)) << res.toString();
    }
};

// 1. AddElectrolytic
TEST_F(ElectrolyticManagerTest, AddElectrolytic_InsertsRow) {
    Electrolytic e{ compId, 6.3, 11.0, 2.5 };
    ASSERT_TRUE(elecMgr.add(e, res)) << res.toString();

    Electrolytic fetched;
    ASSERT_TRUE(elecMgr.getById(compId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.componentId, compId);
    EXPECT_DOUBLE_EQ(fetched.diameter, 6.3);
    EXPECT_DOUBLE_EQ(fetched.height, 11.0);
    EXPECT_DOUBLE_EQ(fetched.leadSpacing, 2.5);
}

// 2. GetElectrolyticById
TEST_F(ElectrolyticManagerTest, GetElectrolyticById_ReturnsCorrectElectrolytic) {
    Electrolytic e{ compId, 10.0, 20.0, 5.0 };
    ASSERT_TRUE(elecMgr.add(e, res)) << res.toString();

    Electrolytic fetched;
    ASSERT_TRUE(elecMgr.getById(compId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.componentId, compId);
    EXPECT_DOUBLE_EQ(fetched.diameter, 10.0);
    EXPECT_DOUBLE_EQ(fetched.height, 20.0);
    EXPECT_DOUBLE_EQ(fetched.leadSpacing, 5.0);
}

// 3. UpdateElectrolytic
TEST_F(ElectrolyticManagerTest, UpdateElectrolytic_ChangesPersist) {
    Electrolytic e{ compId, 8.0, 15.0, 3.5 };
    ASSERT_TRUE(elecMgr.add(e, res)) << res.toString();

    Electrolytic updated = e;
    updated.height = 18.0;
    updated.leadSpacing = 4.0;
    ASSERT_TRUE(elecMgr.update(updated, res)) << res.toString();

    Electrolytic fetched;
    ASSERT_TRUE(elecMgr.getById(compId, fetched, res)) << res.toString();

    EXPECT_DOUBLE_EQ(fetched.height, 18.0);
    EXPECT_DOUBLE_EQ(fetched.leadSpacing, 4.0);
}

// 4. RemoveElectrolytic
TEST_F(ElectrolyticManagerTest, RemoveElectrolytic_DeletesRow) {
    Electrolytic e{ compId, 5.0, 12.0, 2.0 };
    ASSERT_TRUE(elecMgr.add(e, res)) << res.toString();

    ASSERT_TRUE(elecMgr.remove(compId, res)) << res.toString();

    Electrolytic fetched;
    bool got = elecMgr.getById(compId, fetched, res);
    EXPECT_FALSE(got);
}

// 5. CascadeDelete
TEST_F(ElectrolyticManagerTest, CascadeDelete_ComponentAlsoDeletesElectrolytic) {
    Electrolytic e{ compId, 6.0, 13.0, 2.5 };
    ASSERT_TRUE(elecMgr.add(e, res)) << res.toString();

    ASSERT_TRUE(compMgr.deleteComponent(compId, res)) << res.toString();

    Electrolytic fetched;
    bool got = elecMgr.getById(compId, fetched, res);
    EXPECT_FALSE(got);
}
