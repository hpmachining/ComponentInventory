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
    ASSERT_TRUE(compMgr.addComponent(c, res)) << res.toString();

    int compId = compMgr.getByPartNumber(c.partNumber, res);
    ASSERT_GT(compId, 0);

    Resistor r;
    r.componentId = compId;
    r.resistance = 1000.0;
    r.tolerance = 5.0;
    r.powerRating = 0.25;
    r.tempCoefficient = 100.0;
    r.packageTypeId = pkgId;
    r.compositionId = compTypeId;
    r.leadSpacing = 7.5;
    r.voltageRating = 250.0;

    ASSERT_TRUE(resistorMgr.add(r, res)) << res.toString();

    Resistor fetched;
    ASSERT_TRUE(resistorMgr.getByComponentId(compId, fetched, res));
    EXPECT_EQ(fetched.resistance, 1000.0);
}

//
// 2. GetByComponentId_ReturnsCorrectResistor
//
TEST_F(ResistorManagerTest, GetByComponentId_ReturnsCorrectResistor) {
    Component c("TEST_RES_GET", "Test Resistor Get", catId, manId, 5);
    ASSERT_TRUE(compMgr.addComponent(c, res)) << res.toString();

    int compId = compMgr.getByPartNumber(c.partNumber, res);
    ASSERT_GT(compId, 0);

    Resistor r;
    r.componentId = compId;
    r.resistance = 470.0;
    r.tolerance = 1.0;
    r.powerRating = 0.125;
    r.tempCoefficient = 50.0;
    r.packageTypeId = pkgId;
    r.compositionId = compTypeId;

    ASSERT_TRUE(resistorMgr.add(r, res)) << res.toString();

    Resistor fetched;
    ASSERT_TRUE(resistorMgr.getByComponentId(compId, fetched, res));
    EXPECT_DOUBLE_EQ(fetched.resistance, 470.0);
}

//
// 3. UpdateResistor_ChangesValues
//
TEST_F(ResistorManagerTest, UpdateResistor_ChangesValues) {
    Component c("TEST_RES_UPDATE", "Test Resistor Update", catId, manId, 8);
    ASSERT_TRUE(compMgr.addComponent(c, res)) << res.toString();

    int compId = compMgr.getByPartNumber(c.partNumber, res);
    ASSERT_GT(compId, 0);

    Resistor r;
    r.componentId = compId;
    r.resistance = 100.0;
    r.tolerance = 5.0;
    r.powerRating = 0.25;
    r.packageTypeId = pkgId;
    r.compositionId = compTypeId;

    ASSERT_TRUE(resistorMgr.add(r, res)) << res.toString();

    r.resistance = 220.0;
    r.tolerance = 1.0;
    ASSERT_TRUE(resistorMgr.update(r, res)) << res.toString();

    Resistor fetched;
    ASSERT_TRUE(resistorMgr.getByComponentId(compId, fetched, res));
    EXPECT_DOUBLE_EQ(fetched.resistance, 220.0);
    EXPECT_DOUBLE_EQ(fetched.tolerance, 1.0);
}

//
// 4. DeleteResistor_RemovesRow
//
TEST_F(ResistorManagerTest, DeleteResistor_RemovesRow) {
    Component c("TEST_RES_DELETE", "Test Resistor Delete", catId, manId, 3);
    ASSERT_TRUE(compMgr.addComponent(c, res)) << res.toString();

    int compId = compMgr.getByPartNumber(c.partNumber, res);
    ASSERT_GT(compId, 0);

    Resistor r;
    r.componentId = compId;
    r.resistance = 330.0;
    r.packageTypeId = pkgId;
    r.compositionId = compTypeId;

    ASSERT_TRUE(resistorMgr.add(r, res)) << res.toString();
    ASSERT_TRUE(resistorMgr.remove(compId, res)) << res.toString();

    Resistor fetched;
    EXPECT_FALSE(resistorMgr.getByComponentId(compId, fetched, res));
}

//
// 5. ListResistors_ReturnsAllResistors
//
TEST_F(ResistorManagerTest, ListResistors_ReturnsAllResistors) {
    Component c1("TEST_RES_LIST1", "Test Resistor 1", catId, manId, 10);
    Component c2("TEST_RES_LIST2", "Test Resistor 2", catId, manId, 5);

    ASSERT_TRUE(compMgr.addComponent(c1, res));
    ASSERT_TRUE(compMgr.addComponent(c2, res));

    int id1 = compMgr.getByPartNumber(c1.partNumber, res);
    int id2 = compMgr.getByPartNumber(c2.partNumber, res);
    ASSERT_GT(id1, 0);
    ASSERT_GT(id2, 0);

    Resistor r1{ id1, 1000.0, 5.0, 0.25, 100, pkgId, compTypeId, 7.5, 250 };
    Resistor r2{ id2, 2200.0, 1.0, 0.5, 50, pkgId, compTypeId, 7.5, 250 };

    ASSERT_TRUE(resistorMgr.add(r1, res)) << res.toString();
    ASSERT_TRUE(resistorMgr.add(r2, res)) << res.toString();

    std::vector<Resistor> list;
    ASSERT_TRUE(resistorMgr.list(list, res)) << res.toString();

    bool found1 = false, found2 = false;
    for (const auto& r : list) {
        if (r.componentId == id1) found1 = true;
        if (r.componentId == id2) found2 = true;
    }

    EXPECT_TRUE(found1);
    EXPECT_TRUE(found2);
}

//
// 6. AddResistor_WithInvalidComponent_Fails
//
TEST_F(ResistorManagerTest, AddResistor_WithInvalidComponent_Fails) {
    Resistor r;
    r.componentId = 999999;
    r.resistance = 100.0;
    r.packageTypeId = pkgId;
    r.compositionId = compTypeId;

    EXPECT_FALSE(resistorMgr.add(r, res));
}

//
// 7. GetByComponentId_Nonexistent_ReturnsFalse
//
TEST_F(ResistorManagerTest, GetByComponentId_Nonexistent_ReturnsFalse) {
    Resistor r;
    EXPECT_FALSE(resistorMgr.getByComponentId(999999, r, res));
}
