#include "BackendTestFixture.h"
#include "DiodeManager.h"
#include "ComponentManager.h"
#include "DiodePackageManager.h"
#include "DiodeTypeManager.h"
#include "DiodePolarityManager.h"

class DiodeManagerTest : public BackendTestFixture {
protected:
    DiodeManager diodeMgr;
    ComponentManager compMgr;
    DiodePackageManager pkgMgr;
    DiodeTypeManager typeMgr;
    DiodePolarityManager polMgr;

    int catId = 1; // assuming category exists
    int manId = 1; // assuming manufacturer exists

    DiodeManagerTest()
        : diodeMgr(db), compMgr(db), pkgMgr(db), typeMgr(db), polMgr(db) {
    }
};

// 1. AddDiode_InsertsRow
TEST_F(DiodeManagerTest, AddDiode_InsertsRow) {
    Component c("D1000", "Unit test diode", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = compMgr.getByPartNumber("D1000", res);

    int pkgId = pkgMgr.getByName("Axial leaded", res);
    int typeId = typeMgr.getByName("Rectifier", res);
    int polId = polMgr.getByName("Anode-Cathode", res);

    Diode d(compId, pkgId, typeId, polId, 0.7, 1.0, 100.0, 0.01);
    EXPECT_TRUE(diodeMgr.add(d, res));

    Diode fetched;
    EXPECT_TRUE(diodeMgr.getById(compId, fetched, res));
    EXPECT_EQ(fetched.componentId, compId);
}

// 2. AddDuplicateDiode_Fails
TEST_F(DiodeManagerTest, AddDuplicateDiode_Fails) {
    Component c("D1001", "Duplicate diode test", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = compMgr.getByPartNumber("D1001", res);

    int pkgId = pkgMgr.getByName("Axial leaded", res);
    int typeId = typeMgr.getByName("Rectifier", res);
    int polId = polMgr.getByName("Anode-Cathode", res);

    Diode d(compId, pkgId, typeId, polId, 0.7, 1.0, 100.0, 0.01);
    ASSERT_TRUE(diodeMgr.add(d, res));

    // Attempt to add same component again should fail (unique componentId)
    Diode duplicate(compId, pkgId, typeId, polId, 0.5, 0.5, 50.0, 0.005);
    EXPECT_FALSE(diodeMgr.add(duplicate, res));
}

// 3. GetDiodeById_ReturnsCorrectDiode
TEST_F(DiodeManagerTest, GetDiodeById_ReturnsCorrectDiode) {
    Component c("D1002", "GetDiode test", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = compMgr.getByPartNumber("D1002", res);

    int pkgId = pkgMgr.getByName("SMD SOD-123", res);
    int typeId = typeMgr.getByName("Zener", res);
    int polId = polMgr.getByName("Cathode-Anode", res);

    Diode d(compId, pkgId, typeId, polId, 3.3, 0.5, 50.0, 0.02);
    ASSERT_TRUE(diodeMgr.add(d, res));

    Diode fetched;
    ASSERT_TRUE(diodeMgr.getById(compId, fetched, res));
    EXPECT_EQ(fetched.componentId, compId);
    EXPECT_DOUBLE_EQ(fetched.forwardVoltage, 3.3);
}

// 4. UpdateDiode_ChangesPersist
TEST_F(DiodeManagerTest, UpdateDiode_ChangesPersist) {
    Component c("D1003", "UpdateDiode test", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = compMgr.getByPartNumber("D1003", res);

    int pkgId = pkgMgr.getByName("SMD SOD-323", res);
    int typeId = typeMgr.getByName("Schottky", res);
    int polId = polMgr.getByName("Anode-Cathode", res);

    Diode d(compId, pkgId, typeId, polId, 0.3, 2.0, 40.0, 0.05);
    ASSERT_TRUE(diodeMgr.add(d, res));

    // Update diode
    d.forwardVoltage = 2.1;
    d.maxCurrent = 0.02;
    ASSERT_TRUE(diodeMgr.update(d, res));

    Diode fetched;
    ASSERT_TRUE(diodeMgr.getById(compId, fetched, res));
    EXPECT_DOUBLE_EQ(fetched.forwardVoltage, 2.1);
    EXPECT_DOUBLE_EQ(fetched.maxCurrent, 0.02);
}

// 5. RemoveDiode_DeletesRow
TEST_F(DiodeManagerTest, RemoveDiode_DeletesRow) {
    Component c("D1004", "DeleteDiode test", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = compMgr.getByPartNumber("D1004", res);

    int pkgId = pkgMgr.getByName("Axial leaded", res);
    int typeId = typeMgr.getByName("Rectifier", res);
    int polId = polMgr.getByName("Anode-Cathode", res);

    Diode d(compId, pkgId, typeId, polId, 0.7, 1.0, 100.0, 0.01);
    ASSERT_TRUE(diodeMgr.add(d, res));

    EXPECT_TRUE(diodeMgr.remove(compId, res));

    Diode fetched;
    EXPECT_FALSE(diodeMgr.getById(compId, fetched, res));
}

// 6. ListDiodes_ReturnsAll
TEST_F(DiodeManagerTest, ListDiodes_ReturnsAll) {
    // Add first diode
    Component c1("D1005", "ListDiode test 1", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c1, res));
    int compId1 = compMgr.getByPartNumber("D1005", res);

    int pkgId = pkgMgr.getByName("Axial leaded", res);
    int typeId = typeMgr.getByName("Rectifier", res);
    int polId = polMgr.getByName("Anode-Cathode", res);
    ASSERT_TRUE(diodeMgr.add(Diode(compId1, pkgId, typeId, polId, 0.7, 1.0, 100.0, 0.01), res));

    // Add second diode
    Component c2("D1006", "ListDiode test 2", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c2, res));
    int compId2 = compMgr.getByPartNumber("D1006", res);
    ASSERT_TRUE(diodeMgr.add(Diode(compId2, pkgId, typeId, polId, 0.3, 2.0, 40.0, 0.05), res));

    std::vector<Diode> diodes;
    EXPECT_TRUE(diodeMgr.list(diodes, res));
    EXPECT_GE(diodes.size(), 2);  // at least the two just added
}

// 7. GetDiodeById_MissingReturnsFalse
TEST_F(DiodeManagerTest, GetDiodeById_MissingReturnsFalse) {
    Diode fetched;
    EXPECT_FALSE(diodeMgr.getById(-9999, fetched, res));
}
