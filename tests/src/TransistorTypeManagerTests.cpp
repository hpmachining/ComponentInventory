#include "BackendTestFixture.h"
#include "TransistorTypeManager.h"
#include <unordered_set>

// Fixture for TransistorTypeManager
class TransistorTypeManagerTest : public BackendTestFixture {
protected:
    TransistorTypeManager typeMgr;

    TransistorTypeManagerTest()
        : typeMgr(db) {
    }
};

// 1. Add_InsertsRow
TEST_F(TransistorTypeManagerTest, Add_InsertsRow) {
    TransistorType t("TestType");
    EXPECT_TRUE(typeMgr.add(t, res));

    // Verify retrieval by name
    int id = typeMgr.getByName("TestType", res);
    EXPECT_GT(id, 0);

    // Verify retrieval by ID
    TransistorType fetched;
    EXPECT_TRUE(typeMgr.getById(id, fetched, res));
    EXPECT_EQ(fetched.name, "TestType");
}

// 2. Add_DuplicateFails
TEST_F(TransistorTypeManagerTest, Add_DuplicateFails) {
    TransistorType t("DuplicateType");
    ASSERT_TRUE(typeMgr.add(t, res));
    // Second insert with same name should fail
    EXPECT_FALSE(typeMgr.add(t, res));
}

// 3. Remove_RemovesRow
TEST_F(TransistorTypeManagerTest, Remove_RemovesRow) {
    TransistorType t("DeleteType");
    ASSERT_TRUE(typeMgr.add(t, res));
    int id = typeMgr.getByName("DeleteType", res);
    ASSERT_GT(id, 0);

    EXPECT_TRUE(typeMgr.remove(id, res));

    // Verify it no longer exists
    TransistorType fetched;
    EXPECT_FALSE(typeMgr.getById(id, fetched, res));
    EXPECT_EQ(typeMgr.getByName("DeleteType", res), -1);
}

// 4. List_ReturnsAll
TEST_F(TransistorTypeManagerTest, List_ReturnsAll) {
    TransistorType t1("Type1");
    TransistorType t2("Type2");
    ASSERT_TRUE(typeMgr.add(t1, res));
    ASSERT_TRUE(typeMgr.add(t2, res));

    std::vector<TransistorType> types;
    EXPECT_TRUE(typeMgr.list(types, res));
    EXPECT_GE(types.size(), 2u);

    std::unordered_set<std::string> names;
    for (const auto& t : types) names.insert(t.name);

    EXPECT_NE(names.find("Type1"), names.end());
    EXPECT_NE(names.find("Type2"), names.end());
}

// 5. GetByName_NotFoundReturnsMinusOne
TEST_F(TransistorTypeManagerTest, GetByName_NotFoundReturnsMinusOne) {
    EXPECT_EQ(typeMgr.getByName("NonExistentType", res), -1);
}

// 6. GetById_NotFoundReturnsFalse
TEST_F(TransistorTypeManagerTest, GetById_NotFoundReturnsFalse) {
    TransistorType t;
    EXPECT_FALSE(typeMgr.getById(999999, t, res));
}
