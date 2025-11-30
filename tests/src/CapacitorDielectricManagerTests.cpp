#include "BackendTestFixture.h"
#include "CapacitorDielectricManager.h"

class CapacitorDielectricManagerTest : public BackendTestFixture {
protected:
    CapacitorDielectricManager dielMgr;
    CapacitorDielectricManagerTest() : dielMgr(db) {}
};

// 1. AddDielectric_InsertsRow
TEST_F(CapacitorDielectricManagerTest, AddDielectric_InsertsRow) {
    CapacitorDielectric diel("Ceramic");
    ASSERT_TRUE(dielMgr.addDielectric(diel, res)) << res.toString();

    int insertedDielId = db.lastInsertId();
    CapacitorDielectric fetched;
    ASSERT_TRUE(dielMgr.getDielectricById(insertedDielId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedDielId);
    EXPECT_EQ(fetched.name, "Ceramic");
}

// 2. GetDielectricById_ReturnsCorrectDielectric
TEST_F(CapacitorDielectricManagerTest, GetDielectricById_ReturnsCorrectDielectric) {
    CapacitorDielectric diel("Polyester");
    ASSERT_TRUE(dielMgr.addDielectric(diel, res)) << res.toString();
    int insertedDielId = db.lastInsertId();

    CapacitorDielectric fetched;
    ASSERT_TRUE(dielMgr.getDielectricById(insertedDielId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedDielId);
    EXPECT_EQ(fetched.name, "Polyester");
}

// 3. ListDielectrics_ReturnsAllDielectrics
TEST_F(CapacitorDielectricManagerTest, ListDielectrics_ReturnsAllDielectrics) {
    ASSERT_TRUE(dielMgr.addDielectric(CapacitorDielectric("Polypropylene"), res)) << res.toString();
    ASSERT_TRUE(dielMgr.addDielectric(CapacitorDielectric("Tantalum"), res)) << res.toString();

    std::vector<CapacitorDielectric> diels;
    ASSERT_TRUE(dielMgr.listDielectrics(diels, res)) << res.toString();

    EXPECT_GE(diels.size(), 2);

    bool foundPolypropylene = false, foundTantalum = false;
    for (const auto& d : diels) {
        if (d.name == "Polypropylene") foundPolypropylene = true;
        if (d.name == "Tantalum") foundTantalum = true;
    }
    EXPECT_TRUE(foundPolypropylene);
    EXPECT_TRUE(foundTantalum);
}

// 4. DeleteDielectric_RemovesRow
TEST_F(CapacitorDielectricManagerTest, DeleteDielectric_RemovesRow) {
    CapacitorDielectric diel("DeleteMe");
    ASSERT_TRUE(dielMgr.addDielectric(diel, res)) << res.toString();
    int insertedDielId = db.lastInsertId();

    ASSERT_TRUE(dielMgr.deleteDielectric(insertedDielId, res)) << res.toString();

    CapacitorDielectric fetched;
    bool gotDiel = dielMgr.getDielectricById(insertedDielId, fetched, res);
    EXPECT_FALSE(gotDiel);  // should not exist anymore
}
