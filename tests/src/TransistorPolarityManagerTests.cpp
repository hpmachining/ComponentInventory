#include "BackendTestFixture.h"
#include "TransistorPolarityManager.h"
#include <unordered_set>

// Fixture for TransistorPolarityManager
class TransistorPolarityManagerTest : public BackendTestFixture {
protected:
    TransistorPolarityManager polMgr;

    TransistorPolarityManagerTest()
        : polMgr(db) {
    }
};

// 1. Add_InsertsRow
TEST_F(TransistorPolarityManagerTest, Add_InsertsRow) {
    TransistorPolarity p("TestPolarity");
    EXPECT_TRUE(polMgr.add(p, res));

    // Verify retrieval by name
    int id = polMgr.getByName("TestPolarity", res);
    EXPECT_GT(id, 0);

    // Verify retrieval by ID
    TransistorPolarity fetched;
    EXPECT_TRUE(polMgr.getById(id, fetched, res));
    EXPECT_EQ(fetched.name, "TestPolarity");
}

// 2. Add_DuplicateFails
TEST_F(TransistorPolarityManagerTest, Add_DuplicateFails) {
    TransistorPolarity p("DuplicatePolarity");
    ASSERT_TRUE(polMgr.add(p, res));
    // Second insert with same name should fail
    EXPECT_FALSE(polMgr.add(p, res));
}

// 3. Delete_RemovesRow
TEST_F(TransistorPolarityManagerTest, Delete_RemovesRow) {
    TransistorPolarity p("DeletePolarity");
    ASSERT_TRUE(polMgr.add(p, res));
    int id = polMgr.getByName("DeletePolarity", res);
    ASSERT_GT(id, 0);

    EXPECT_TRUE(polMgr.remove(id, res));

    // Verify it no longer exists
    TransistorPolarity fetched;
    EXPECT_FALSE(polMgr.getById(id, fetched, res));
    EXPECT_EQ(polMgr.getByName("DeletePolarity", res), -1);
}

// 4. List_ReturnsAll
TEST_F(TransistorPolarityManagerTest, List_ReturnsAll) {
    TransistorPolarity p1("Polarity1");
    TransistorPolarity p2("Polarity2");
    ASSERT_TRUE(polMgr.add(p1, res));
    ASSERT_TRUE(polMgr.add(p2, res));

    std::vector<TransistorPolarity> polarities;
    EXPECT_TRUE(polMgr.list(polarities, res));
    EXPECT_GE(polarities.size(), 2u);

    std::unordered_set<std::string> names;
    for (const auto& p : polarities) names.insert(p.name);

    EXPECT_NE(names.find("Polarity1"), names.end());
    EXPECT_NE(names.find("Polarity2"), names.end());
}

// 5. GetByName_NotFoundReturnsMinusOne
TEST_F(TransistorPolarityManagerTest, GetByName_NotFoundReturnsMinusOne) {
    EXPECT_EQ(polMgr.getByName("NonExistentPolarity", res), -1);
}

// 6. GetById_NotFoundReturnsFalse
TEST_F(TransistorPolarityManagerTest, GetById_NotFoundReturnsFalse) {
    TransistorPolarity p;
    EXPECT_FALSE(polMgr.getById(999999, p, res));
}
