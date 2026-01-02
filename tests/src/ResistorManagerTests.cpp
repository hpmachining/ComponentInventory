#include "BackendTestFixture.h"
#include "ResistorManager.h"
#include "ComponentManager.h"
#include "ResistorPackageManager.h"
#include "ResistorCompositionManager.h"

class ResistorManagerTest : public BackendTestFixture {
protected:
    ResistorManager resistorMgr;
    ComponentManager compMgr;
    ResistorPackageManager pkgMgr;
    ResistorCompositionManager compTypeMgr;

    int pkgId{ 0 };
    int compTypeId{ 0 };

    ResistorManagerTest()
        : resistorMgr(db),
        compMgr(db),
        pkgMgr(db),
        compTypeMgr(db) {
    }

    void SetUp() override {
        BackendTestFixture::SetUp();

        // Package
        ResistorPackage pkg("TEST_AXIAL");
        ASSERT_TRUE(pkgMgr.add(pkg, res)) << res.toString();
        pkgId = pkgMgr.getByName("TEST_AXIAL", res);
        ASSERT_GT(pkgId, 0);

        // Composition
        ResistorComposition comp("TEST_CARBON");
        ASSERT_TRUE(compTypeMgr.add(comp, res)) << res.toString();
        compTypeId = compTypeMgr.getByName("TEST_CARBON", res);
        ASSERT_GT(compTypeId, 0);
    }
};

//
// 1. AddResistor_InsertsRow
//
TEST_F(ResistorManagerTest, AddResistor_InsertsRow) {
    Component c("TEST_RES_ADD", "Test Resistor Add", catId, manId, 10);
    ASSERT_TRUE(compMgr.add(c, res)) << res.toString();
    ASSERT_GT(c.id, 0);

    // Use parameterized constructor
    Resistor r(
        c.id,          // componentId
        1000.0,        // resistance
        5.0,           // tolerance
        0.25,          // powerRating
        true,          // hasTempCoeff
        100.0,         // tempCoeffMin
        100.0,         // tempCoeffMax
        true,          // hasTempRange
        -55.0,         // tempMin
        125.0,         // tempMax
        pkgId,         // packageTypeId
        compTypeId,    // compositionId
        7.5,           // leadSpacing
        250.0          // voltageRating
    );

    ASSERT_TRUE(resistorMgr.add(r, res)) << res.toString();

    Resistor fetched;
    ASSERT_TRUE(resistorMgr.getByComponentId(c.id, fetched, res));
    EXPECT_DOUBLE_EQ(fetched.resistance, 1000.0);
    EXPECT_TRUE(fetched.hasTempCoeff);
    EXPECT_DOUBLE_EQ(fetched.tempCoeffMin, 100.0);
    EXPECT_DOUBLE_EQ(fetched.tempCoeffMax, 100.0);
    EXPECT_TRUE(fetched.hasTempRange);
    EXPECT_DOUBLE_EQ(fetched.tempMin, -55.0);
    EXPECT_DOUBLE_EQ(fetched.tempMax, 125.0);
}

//
// 2. GetByComponentId_ReturnsCorrectResistor
//
TEST_F(ResistorManagerTest, GetByComponentId_ReturnsCorrectResistor) {
    Component c("TEST_RES_GET", "Test Resistor Get", catId, manId, 5);
    ASSERT_TRUE(compMgr.add(c, res)) << res.toString();
    ASSERT_GT(c.id, 0);

    Resistor r(
        c.id,
        470.0,
        1.0,
        0.125,
        true,       // hasTempCoeff
        50.0,
        50.0,
        false,      // hasTempRange
        0.0,
        0.0,
        pkgId,
        compTypeId,
        5.0,
        200.0
    );

    ASSERT_TRUE(resistorMgr.add(r, res)) << res.toString();

    Resistor fetched;
    ASSERT_TRUE(resistorMgr.getByComponentId(c.id, fetched, res));
    EXPECT_DOUBLE_EQ(fetched.resistance, 470.0);
    EXPECT_TRUE(fetched.hasTempCoeff);
    EXPECT_FALSE(fetched.hasTempRange);
}

//
// 3. UpdateResistor_ChangesValues
//
TEST_F(ResistorManagerTest, UpdateResistor_ChangesValues) {
    Component c("TEST_RES_UPDATE", "Test Resistor Update", catId, manId, 8);
    ASSERT_TRUE(compMgr.add(c, res)) << res.toString();
    ASSERT_GT(c.id, 0);

    Resistor r(
        c.id,
        100.0,
        5.0,
        0.25,
        false,      // no TCR
        0.0,
        0.0,
        false,      // no temp range
        0.0,
        0.0,
        pkgId,
        compTypeId,
        5.0,
        200.0
    );

    ASSERT_TRUE(resistorMgr.add(r, res)) << res.toString();

    r.resistance = 220.0;
    r.tolerance = 1.0;
    ASSERT_TRUE(resistorMgr.update(r, res)) << res.toString();

    Resistor fetched;
    ASSERT_TRUE(resistorMgr.getByComponentId(c.id, fetched, res));
    EXPECT_DOUBLE_EQ(fetched.resistance, 220.0);
    EXPECT_DOUBLE_EQ(fetched.tolerance, 1.0);
    EXPECT_FALSE(fetched.hasTempCoeff);
    EXPECT_FALSE(fetched.hasTempRange);
}

//
// 4. DeleteResistor_RemovesRow
//
TEST_F(ResistorManagerTest, DeleteResistor_RemovesRow) {
    Component c("TEST_RES_DELETE", "Test Resistor Delete", catId, manId, 3);
    ASSERT_TRUE(compMgr.add(c, res)) << res.toString();
    ASSERT_GT(c.id, 0);

    Resistor r(
        c.id,
        330.0,
        5.0,
        0.25,
        false,
        0.0,
        0.0,
        false,
        0.0,
        0.0,
        pkgId,
        compTypeId,
        5.0,
        200.0
    );

    ASSERT_TRUE(resistorMgr.add(r, res)) << res.toString();
    ASSERT_TRUE(resistorMgr.remove(c.id, res)) << res.toString();

    Resistor fetched;
    EXPECT_FALSE(resistorMgr.getByComponentId(c.id, fetched, res));
}

//
// 5. ListResistors_ReturnsAllResistors
//
TEST_F(ResistorManagerTest, ListResistors_ReturnsAllResistors) {
    Component c1("TEST_RES_LIST1", "Test Resistor 1", catId, manId, 10);
    Component c2("TEST_RES_LIST2", "Test Resistor 2", catId, manId, 5);

    ASSERT_TRUE(compMgr.add(c1, res));
    ASSERT_TRUE(compMgr.add(c2, res));

    ASSERT_GT(c1.id, 0);
    ASSERT_GT(c2.id, 0);

    Resistor r1(
        c1.id,
        1000.0,
        5.0,
        0.25,
        true,
        100.0,
        100.0,
        true,
        -55.0,
        125.0,
        pkgId,
        compTypeId,
        7.5,
        250.0
    );

    Resistor r2(
        c2.id,
        2200.0,
        1.0,
        0.5,
        false,  // no TCR
        0.0,
        0.0,
        false,  // no temp range
        0.0,
        0.0,
        pkgId,
        compTypeId,
        7.5,
        250.0
    );

    ASSERT_TRUE(resistorMgr.add(r1, res)) << res.toString();
    ASSERT_TRUE(resistorMgr.add(r2, res)) << res.toString();

    std::vector<Resistor> list;
    ASSERT_TRUE(resistorMgr.list(list, res)) << res.toString();

    bool found1 = false, found2 = false;
    for (const auto& r : list) {
        if (r.componentId == c1.id) found1 = true;
        if (r.componentId == c2.id) found2 = true;
    }

    EXPECT_TRUE(found1);
    EXPECT_TRUE(found2);
}

//
// 6. AddResistor_WithNoTCROrTempRange
//
TEST_F(ResistorManagerTest, AddResistor_WithNoTCROrTempRange) {
    Component c("TEST_RES_NOTCR", "Test Resistor No TCR", catId, manId, 3);
    ASSERT_TRUE(compMgr.add(c, res)) << res.toString();
    ASSERT_GT(c.id, 0);

    Resistor r(
        c.id,
        560.0,
        5.0,
        0.25,
        false,      // no TCR
        0.0,
        0.0,
        false,      // no temp range
        0.0,
        0.0,
        pkgId,
        compTypeId,
        5.0,
        200.0
    );

    ASSERT_TRUE(resistorMgr.add(r, res)) << res.toString();

    Resistor fetched;
    ASSERT_TRUE(resistorMgr.getByComponentId(c.id, fetched, res));
    EXPECT_FALSE(fetched.hasTempCoeff);
    EXPECT_FALSE(fetched.hasTempRange);
}

//
// 7. AddResistor_WithInvalidComponent_Fails
//
TEST_F(ResistorManagerTest, AddResistor_WithInvalidComponent_Fails) {
    Resistor r(
        999999,  // invalid component
        100.0,
        5.0,
        0.25,
        false,
        0.0,
        0.0,
        false,
        0.0,
        0.0,
        pkgId,
        compTypeId,
        5.0,
        200.0
    );

    EXPECT_FALSE(resistorMgr.add(r, res));
}

//
// 8. GetByComponentId_Nonexistent_ReturnsFalse
//
TEST_F(ResistorManagerTest, GetByComponentId_Nonexistent_ReturnsFalse) {
    Resistor r;
    EXPECT_FALSE(resistorMgr.getByComponentId(999999, r, res));
}
