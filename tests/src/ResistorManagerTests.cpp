#include "BackendTestFixture.h"
#include "ResistorManager.h"
#include "ComponentManager.h"
#include "ResistorPackageManager.h"
#include "ResistorCompositionManager.h"

class ResistorManagerTest : public BackendTestFixture {
protected:
    ResistorManager resMgr;
    ComponentManager compMgr;
    ResistorPackageManager pkgMgr;
    ResistorCompositionManager compoMgr;

    int pkgId;
    int compoId;
    int compId;

    ResistorManagerTest()
        : resMgr(db), compMgr(db), pkgMgr(db), compoMgr(db),
        pkgId(0), compoId(0), compId(0) {
    }

    void SetUp() override {
        BackendTestFixture::SetUp(); // base fixture seeds lookups

        // Resolve seeded lookup IDs
        pkgId = pkgMgr.getByName("0805", res);
        ASSERT_GT(pkgId, 0);

        compoId = compoMgr.getByName("Carbon Film", res);
        ASSERT_GT(compoId, 0);

        // Seed a component specific to resistor tests
        Component comp("R-100", "Seed resistor component", catId, manId, 100, "Seed notes");
        ASSERT_TRUE(compMgr.addComponent(comp, res)) << res.toString();
        compId = compMgr.getByPartNumber("R-100", res);
        ASSERT_GT(compId, 0);
    }
};

// 1. AddResistor_InsertsRow
TEST_F(ResistorManagerTest, AddResistor_InsertsRow) {
    Resistor r;
    r.componentId = compId;
    r.resistance = 1000.0;
    r.tolerance = 5.0;
    r.powerRating = 0.25;
    r.tempCoefficient = 100.0;
    r.packageTypeId = pkgId;
    r.compositionId = compoId;
    r.leadSpacing = 2.54;
    r.voltageRating = 50.0;

    ASSERT_TRUE(resMgr.addResistor(r, res)) << res.toString();

    Resistor fetched;
    ASSERT_TRUE(resMgr.getResistorByComponentId(compId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.componentId, compId);
    EXPECT_DOUBLE_EQ(fetched.resistance, 1000.0);
    EXPECT_DOUBLE_EQ(fetched.tolerance, 5.0);
    EXPECT_DOUBLE_EQ(fetched.powerRating, 0.25);
    EXPECT_DOUBLE_EQ(fetched.tempCoefficient, 100.0);
    EXPECT_EQ(fetched.packageTypeId, pkgId);
    EXPECT_EQ(fetched.compositionId, compoId);
    EXPECT_DOUBLE_EQ(fetched.leadSpacing, 2.54);
    EXPECT_DOUBLE_EQ(fetched.voltageRating, 50.0);
}

// 2. GetResistorByComponentId_ReturnsCorrectResistor
TEST_F(ResistorManagerTest, GetResistorByComponentId_ReturnsCorrectResistor) {
    Resistor r;
    r.componentId = compId;
    r.resistance = 220.0;
    r.tolerance = 1.0;
    r.powerRating = 0.125;
    r.tempCoefficient = 50.0;
    r.packageTypeId = pkgId;
    r.compositionId = compoId;
    r.leadSpacing = 1.27;
    r.voltageRating = 25.0;

    ASSERT_TRUE(resMgr.addResistor(r, res)) << res.toString();

    Resistor fetched;
    ASSERT_TRUE(resMgr.getResistorByComponentId(compId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.componentId, compId);
    EXPECT_DOUBLE_EQ(fetched.resistance, 220.0);
    EXPECT_DOUBLE_EQ(fetched.tolerance, 1.0);
    EXPECT_DOUBLE_EQ(fetched.powerRating, 0.125);
    EXPECT_DOUBLE_EQ(fetched.tempCoefficient, 50.0);
    EXPECT_EQ(fetched.packageTypeId, pkgId);
    EXPECT_EQ(fetched.compositionId, compoId);
    EXPECT_DOUBLE_EQ(fetched.leadSpacing, 1.27);
    EXPECT_DOUBLE_EQ(fetched.voltageRating, 25.0);
}

// 3. UpdateResistor_ChangesPersist
TEST_F(ResistorManagerTest, UpdateResistor_ChangesPersist) {
    Resistor r;
    r.componentId = compId;
    r.resistance = 470.0;
    r.tolerance = 10.0;
    r.powerRating = 0.5;
    r.tempCoefficient = 200.0;
    r.packageTypeId = pkgId;
    r.compositionId = compoId;
    r.leadSpacing = 5.0;
    r.voltageRating = 100.0;

    ASSERT_TRUE(resMgr.addResistor(r, res)) << res.toString();

    Resistor updated = r;
    updated.tolerance = 2.0;
    updated.powerRating = 1.0;

    ASSERT_TRUE(resMgr.updateResistor(updated, res)) << res.toString();

    Resistor fetched;
    ASSERT_TRUE(resMgr.getResistorByComponentId(compId, fetched, res)) << res.toString();

    EXPECT_DOUBLE_EQ(fetched.tolerance, 2.0);
    EXPECT_DOUBLE_EQ(fetched.powerRating, 1.0);
}

// 4. DeleteResistor_RemovesRow
TEST_F(ResistorManagerTest, DeleteResistor_RemovesRow) {
    Resistor r;
    r.componentId = compId;
    r.resistance = 330.0;
    r.tolerance = 5.0;
    r.powerRating = 0.25;
    r.tempCoefficient = 100.0;
    r.packageTypeId = pkgId;
    r.compositionId = compoId;
    r.leadSpacing = 2.54;
    r.voltageRating = 50.0;

    ASSERT_TRUE(resMgr.addResistor(r, res)) << res.toString();

    ASSERT_TRUE(resMgr.deleteResistor(compId, res)) << res.toString();

    Resistor fetched;
    bool got = resMgr.getResistorByComponentId(compId, fetched, res);
    EXPECT_FALSE(got);  // should no longer exist
}

// 5. ForeignKey_ComponentAndPackageAndComposition_Valid
TEST_F(ResistorManagerTest, ForeignKey_ComponentAndPackageAndComposition_Valid) {
    // Valid insert
    Resistor valid;
    valid.componentId = compId;
    valid.resistance = 100.0;
    valid.tolerance = 5.0;
    valid.powerRating = 0.25;
    valid.tempCoefficient = 100.0;
    valid.packageTypeId = pkgId;
    valid.compositionId = compoId;
    valid.leadSpacing = 2.54;
    valid.voltageRating = 50.0;
    ASSERT_TRUE(resMgr.addResistor(valid, res)) << res.toString();

    // Invalid component
    Resistor badComp = valid;
    badComp.componentId = 999999;
    EXPECT_FALSE(resMgr.addResistor(badComp, res));

    // Invalid package
    Resistor badPkg = valid;
    badPkg.packageTypeId = 999999;
    EXPECT_FALSE(resMgr.addResistor(badPkg, res));

    // Invalid composition
    Resistor badCompo = valid;
    badCompo.compositionId = 999999;
    EXPECT_FALSE(resMgr.addResistor(badCompo, res));
}
