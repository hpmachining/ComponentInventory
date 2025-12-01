#include "BackendTestFixture.h"
#include "CapacitorDielectricManager.h"

class CapacitorDielectricManagerTest : public BackendTestFixture {
protected:
    CapacitorDielectricManager dielMgr;
    CapacitorDielectricManagerTest() : dielMgr(db) {}
};

// 1. AddDielectric_InsertsRow
TEST_F(CapacitorDielectricManagerTest, AddDielectric_InsertsRow) {
    // Use a unique test-only name to avoid collision with seeded dielectrics
    CapacitorDielectric diel("TestDiel_Add");
    ASSERT_TRUE(dielMgr.addDielectric(diel, res)) << res.toString();

    int insertedDielId = dielMgr.getByName("TestDiel_Add", res);
    ASSERT_GT(insertedDielId, 0);

    CapacitorDielectric fetched;
    ASSERT_TRUE(dielMgr.getDielectricById(insertedDielId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedDielId);
    EXPECT_EQ(fetched.name, "TestDiel_Add");
}

// 2. GetDielectricById_ReturnsCorrectDielectric
TEST_F(CapacitorDielectricManagerTest, GetDielectricById_ReturnsCorrectDielectric) {
    CapacitorDielectric diel("TestDiel_Get");
    ASSERT_TRUE(dielMgr.addDielectric(diel, res)) << res.toString();

    int insertedDielId = dielMgr.getByName("TestDiel_Get", res);
    ASSERT_GT(insertedDielId, 0);

    CapacitorDielectric fetched;
    ASSERT_TRUE(dielMgr.getDielectricById(insertedDielId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedDielId);
    EXPECT_EQ(fetched.name, "TestDiel_Get");
}

// 3. ListDielectrics_ReturnsAllDielectrics
TEST_F(CapacitorDielectricManagerTest, ListDielectrics_ReturnsAllDielectrics) {
    // Add a unique dielectric
    ASSERT_TRUE(dielMgr.addDielectric(CapacitorDielectric("TestDiel_List"), res)) << res.toString();

    std::vector<CapacitorDielectric> diels;
    ASSERT_TRUE(dielMgr.listDielectrics(diels, res)) << res.toString();

    bool foundPolypropylene = false, foundTestDiel = false;
    for (const auto& d : diels) {
        if (d.name == "Polypropylene") foundPolypropylene = true; // seeded canonical
        if (d.name == "TestDiel_List") foundTestDiel = true;      // test-only
    }
    EXPECT_TRUE(foundPolypropylene);
    EXPECT_TRUE(foundTestDiel);
}

// 4. DeleteDielectric_RemovesRow
TEST_F(CapacitorDielectricManagerTest, DeleteDielectric_RemovesRow) {
    CapacitorDielectric diel("TestDiel_Delete");
    ASSERT_TRUE(dielMgr.addDielectric(diel, res)) << res.toString();

    int insertedDielId = dielMgr.getByName("TestDiel_Delete", res);
    ASSERT_GT(insertedDielId, 0);

    ASSERT_TRUE(dielMgr.deleteDielectric(insertedDielId, res)) << res.toString();

    CapacitorDielectric fetched;
    bool gotDiel = dielMgr.getDielectricById(insertedDielId, fetched, res);
    EXPECT_FALSE(gotDiel);  // should not exist anymore
}
