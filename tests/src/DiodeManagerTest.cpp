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
    ASSERT_GT(pkgId, 0);
    int typeId = typeMgr.getByName("Rectifier", res);
    ASSERT_GT(typeId, 0);
    int polId = polMgr.getByName("Anode-Cathode", res);
    ASSERT_GT(polId, 0);

    Diode d{ compId, pkgId, typeId, polId, 0.7, 1.0, 100.0, 0.01 };
    EXPECT_TRUE(diodeMgr.addDiode(d, res));

    Diode fetched;
    EXPECT_TRUE(diodeMgr.getDiodeByComponentId(compId, fetched, res));
    EXPECT_EQ(fetched.componentId, compId);
    EXPECT_EQ(fetched.packageId, pkgId);
    EXPECT_EQ(fetched.typeId, typeId);
    EXPECT_EQ(fetched.polarityId, polId);
    EXPECT_DOUBLE_EQ(fetched.forwardVoltage, 0.7);
    EXPECT_DOUBLE_EQ(fetched.maxCurrent, 1.0);
    EXPECT_DOUBLE_EQ(fetched.maxReverseVoltage, 100.0);
    EXPECT_DOUBLE_EQ(fetched.reverseLeakage, 0.01);
}

// 2. DeleteDiode_RemovesRow
TEST_F(DiodeManagerTest, DeleteDiode_RemovesRow) {
    Component c("D1001", "Unit test diode", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = compMgr.getByPartNumber("D1001", res);

    int pkgId = pkgMgr.getByName("SMD SOD-123", res);
    int typeId = typeMgr.getByName("Zener", res);
    int polId = polMgr.getByName("Cathode-Anode", res);

    Diode d{ compId, pkgId, typeId, polId, 3.3, 0.5, 50.0, 0.02 };
    ASSERT_TRUE(diodeMgr.addDiode(d, res));

    EXPECT_TRUE(diodeMgr.deleteDiode(compId, res));

    Diode fetched;
    EXPECT_FALSE(diodeMgr.getDiodeByComponentId(compId, fetched, res));
}

// 3. UpdateDiode_ChangesPersist
TEST_F(DiodeManagerTest, UpdateDiode_ChangesPersist) {
    Component c("D1002", "Unit test diode", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c, res));
    int compId = compMgr.getByPartNumber("D1002", res);

    int pkgId = pkgMgr.getByName("SMD SOD-323", res);
    int typeId = typeMgr.getByName("Schottky", res);
    int polId = polMgr.getByName("Anode-Cathode", res);

    Diode d{ compId, pkgId, typeId, polId, 0.3, 2.0, 40.0, 0.05 };
    ASSERT_TRUE(diodeMgr.addDiode(d, res));

    // Update to LED type with new ratings
    d.typeId = typeMgr.getByName("LED", res);
    d.forwardVoltage = 2.1;
    d.maxCurrent = 0.02;
    d.maxReverseVoltage = 5.0;
    d.reverseLeakage = 0.001;
    ASSERT_TRUE(diodeMgr.updateDiode(d, res));

    Diode fetched;
    ASSERT_TRUE(diodeMgr.getDiodeByComponentId(compId, fetched, res));
    EXPECT_EQ(fetched.typeId, d.typeId);
    EXPECT_DOUBLE_EQ(fetched.forwardVoltage, 2.1);
    EXPECT_DOUBLE_EQ(fetched.maxCurrent, 0.02);
    EXPECT_DOUBLE_EQ(fetched.maxReverseVoltage, 5.0);
    EXPECT_DOUBLE_EQ(fetched.reverseLeakage, 0.001);
}

// 4. ListDiodes_ReturnsAll
TEST_F(DiodeManagerTest, ListDiodes_ReturnsAll) {
    Component c1("D1003", "Unit test diode", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c1, res));
    int compId1 = compMgr.getByPartNumber("D1003", res);

    int pkgId = pkgMgr.getByName("Axial leaded", res);
    int typeId = typeMgr.getByName("Rectifier", res);
    int polId = polMgr.getByName("Anode-Cathode", res);
    ASSERT_TRUE(diodeMgr.addDiode(Diode{ compId1, pkgId, typeId, polId, 0.7, 1.0, 100.0, 0.01 }, res));

    Component c2("D1004", "Unit test diode", catId, manId, 1);
    ASSERT_TRUE(compMgr.addComponent(c2, res));
    int compId2 = compMgr.getByPartNumber("D1004", res);

    ASSERT_TRUE(diodeMgr.addDiode(Diode{ compId2, pkgId, typeId, polId, 0.3, 2.0, 40.0, 0.05 }, res));

    std::vector<Diode> ds;
    EXPECT_TRUE(diodeMgr.listDiodes(ds, res));
    EXPECT_EQ(ds.size(), 2);

    // Verify ratings are included
    EXPECT_DOUBLE_EQ(ds[0].forwardVoltage, 0.7);
    EXPECT_DOUBLE_EQ(ds[0].maxCurrent, 1.0);
    EXPECT_DOUBLE_EQ(ds[0].maxReverseVoltage, 100.0);
    EXPECT_DOUBLE_EQ(ds[0].reverseLeakage, 0.01);

    EXPECT_DOUBLE_EQ(ds[1].forwardVoltage, 0.3);
    EXPECT_DOUBLE_EQ(ds[1].maxCurrent, 2.0);
    EXPECT_DOUBLE_EQ(ds[1].maxReverseVoltage, 40.0);
    EXPECT_DOUBLE_EQ(ds[1].reverseLeakage, 0.05);
}
