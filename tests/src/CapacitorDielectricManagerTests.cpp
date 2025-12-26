#include "BackendTestFixture.h"
#include "CapacitorDielectricManager.h"

#include <unordered_set>

class CapacitorDielectricManagerTest : public BackendTestFixture {
protected:
    CapacitorDielectricManager dielMgr;
    CapacitorDielectricManagerTest() : dielMgr(db) {}
};

// 1. Add_InsertsRow
TEST_F(CapacitorDielectricManagerTest, Add_InsertsRow) {
    CapacitorDielectric diel("TestDiel_Add");
    ASSERT_TRUE(dielMgr.add(diel, res));

    int id = dielMgr.getByName("TestDiel_Add", res);
    ASSERT_GT(id, 0);

    CapacitorDielectric fetched;
    ASSERT_TRUE(dielMgr.getById(id, fetched, res));
    EXPECT_EQ(fetched.id, id);
    EXPECT_EQ(fetched.name, "TestDiel_Add");
}

// 2. Add_DuplicateFails
TEST_F(CapacitorDielectricManagerTest, Add_DuplicateFails) {
    CapacitorDielectric diel("DuplicateDiel");
    ASSERT_TRUE(dielMgr.add(diel, res));
    EXPECT_FALSE(dielMgr.add(diel, res)); // duplicate
}

// 3. Remove_RemovesRow
TEST_F(CapacitorDielectricManagerTest, Remove_RemovesRow) {
    CapacitorDielectric diel("TestDiel_Remove");
    ASSERT_TRUE(dielMgr.add(diel, res));

    int id = dielMgr.getByName("TestDiel_Remove", res);
    ASSERT_GT(id, 0);

    ASSERT_TRUE(dielMgr.remove(id, res));

    CapacitorDielectric fetched;
    EXPECT_FALSE(dielMgr.getById(id, fetched, res));
    EXPECT_EQ(dielMgr.getByName("TestDiel_Remove", res), -1);
}

// 4. List_ReturnsAll
TEST_F(CapacitorDielectricManagerTest, List_ReturnsAll) {
    ASSERT_TRUE(dielMgr.add(CapacitorDielectric("Type1"), res));
    ASSERT_TRUE(dielMgr.add(CapacitorDielectric("Type2"), res));

    std::vector<CapacitorDielectric> diels;
    ASSERT_TRUE(dielMgr.list(diels, res));

    std::unordered_set<std::string> names;
    for (auto& d : diels) names.insert(d.name);

    EXPECT_NE(names.find("Type1"), names.end());
    EXPECT_NE(names.find("Type2"), names.end());
}

// 5. GetByName_NotFoundReturnsMinusOne
TEST_F(CapacitorDielectricManagerTest, GetByName_NotFoundReturnsMinusOne) {
    EXPECT_EQ(dielMgr.getByName("NonExistent", res), -1);
}

// 6. GetById_NotFoundReturnsFalse
TEST_F(CapacitorDielectricManagerTest, GetById_NotFoundReturnsFalse) {
    CapacitorDielectric diel;
    EXPECT_FALSE(dielMgr.getById(999999, diel, res));
}
