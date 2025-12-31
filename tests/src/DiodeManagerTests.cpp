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

// Helper to create a diode component and add it
static int createDiodeComponent(ComponentManager& compMgr,
    const std::string& partNumber,
    int catId, int manId,
    DbResult& res)
{
    Component c(partNumber, "Unit test diode", catId, manId, 1);

    if (!compMgr.add(c, res)) {
        ADD_FAILURE() << "Failed to add component: " << res.toString();
        return -1;
    }

    if (c.id <= 0) {
        ADD_FAILURE() << "add() succeeded but returned invalid id";
        return -1;
    }

    return c.id;
}

// 1. AddDiode_InsertsRow
TEST_F(DiodeManagerTest, AddDiode_InsertsRow) {
    int compId = createDiodeComponent(compMgr, "D1000", catId, manId, res);

    int pkgId = pkgMgr.getByName("Axial leaded", res);
    int typeId = typeMgr.getByName("Rectifier", res);
    int polId = polMgr.getByName("Anode-Cathode", res);

    Diode d(compId, pkgId, typeId, polId, 0.7, 1.0, 100.0, 0.01);
    ASSERT_TRUE(diodeMgr.add(d, res));

    Diode fetched;
    ASSERT_TRUE(diodeMgr.getById(compId, fetched, res));
    EXPECT_EQ(fetched.componentId, compId);
}

// 2. AddDuplicateDiode_Fails
TEST_F(DiodeManagerTest, AddDuplicateDiode_Fails) {
    int compId = createDiodeComponent(compMgr, "D1001", catId, manId, res);

    int pkgId = pkgMgr.getByName("Axial leaded", res);
    int typeId = typeMgr.getByName("Rectifier", res);
    int polId = polMgr.getByName("Anode-Cathode", res);

    Diode d(compId, pkgId, typeId, polId, 0.7, 1.0, 100.0, 0.01);
    ASSERT_TRUE(diodeMgr.add(d, res));

    Diode duplicate(compId, pkgId, typeId, polId, 0.5, 0.5, 50.0, 0.005);
    EXPECT_FALSE(diodeMgr.add(duplicate, res));
}

// 3. GetDiodeById_ReturnsCorrectDiode
TEST_F(DiodeManagerTest, GetDiodeById_ReturnsCorrectDiode) {
    int compId = createDiodeComponent(compMgr, "D1002", catId, manId, res);

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
    int compId = createDiodeComponent(compMgr, "D1003", catId, manId, res);

    int pkgId = pkgMgr.getByName("SMD SOD-323", res);
    int typeId = typeMgr.getByName("Schottky", res);
    int polId = polMgr.getByName("Anode-Cathode", res);

    Diode d(compId, pkgId, typeId, polId, 0.3, 2.0, 40.0, 0.05);
    ASSERT_TRUE(diodeMgr.add(d, res));

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
    int compId = createDiodeComponent(compMgr, "D1004", catId, manId, res);

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
    int compId1 = createDiodeComponent(compMgr, "D1005", catId, manId, res);
    int compId2 = createDiodeComponent(compMgr, "D1006", catId, manId, res);

    int pkgId = pkgMgr.getByName("Axial leaded", res);
    int typeId = typeMgr.getByName("Rectifier", res);
    int polId = polMgr.getByName("Anode-Cathode", res);

    ASSERT_TRUE(diodeMgr.add(Diode(compId1, pkgId, typeId, polId, 0.7, 1.0, 100.0, 0.01), res));
    ASSERT_TRUE(diodeMgr.add(Diode(compId2, pkgId, typeId, polId, 0.3, 2.0, 40.0, 0.05), res));

    std::vector<Diode> diodes;
    ASSERT_TRUE(diodeMgr.list(diodes, res));
    EXPECT_GE(diodes.size(), 2);
}

// 7. GetDiodeById_MissingReturnsFalse
TEST_F(DiodeManagerTest, GetDiodeById_MissingReturnsFalse) {
    Diode fetched;
    EXPECT_FALSE(diodeMgr.getById(-9999, fetched, res));
}
