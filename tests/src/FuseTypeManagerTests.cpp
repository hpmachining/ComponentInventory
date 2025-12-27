#include "BackendTestFixture.h"
#include "FuseTypeManager.h"

class FuseTypeManagerTest : public BackendTestFixture {
protected:
    FuseTypeManager typeMgr;

    FuseTypeManagerTest()
        : typeMgr(db) {
    }
};

// 1. AddFuseType_InsertsRow
TEST_F(FuseTypeManagerTest, AddFuseType_InsertsRow) {
    FuseType type("TestFuseType_Add");
    ASSERT_TRUE(typeMgr.add(type, res)) << res.toString();

    int id = typeMgr.getByName("TestFuseType_Add", res);
    ASSERT_GT(id, 0);

    FuseType fetched;
    ASSERT_TRUE(typeMgr.getById(id, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, id);
    EXPECT_EQ(fetched.name, "TestFuseType_Add");
}

// 2. GetFuseTypeById_ReturnsCorrectType
TEST_F(FuseTypeManagerTest, GetFuseTypeById_ReturnsCorrectType) {
    ASSERT_TRUE(typeMgr.add(FuseType("TestFuseType_Get"), res)) << res.toString();

    int id = typeMgr.getByName("TestFuseType_Get", res);
    ASSERT_GT(id, 0);

    FuseType fetched;
    ASSERT_TRUE(typeMgr.getById(id, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, id);
    EXPECT_EQ(fetched.name, "TestFuseType_Get");
}

// 3. ListFuseTypes_ReturnsAllTypes
TEST_F(FuseTypeManagerTest, ListFuseTypes_ReturnsAllTypes) {
    const std::string addedName = "TestFuseType_List";
    ASSERT_TRUE(typeMgr.add(FuseType(addedName), res)) << res.toString();

    std::vector<FuseType> types;
    ASSERT_TRUE(typeMgr.list(types, res)) << res.toString();

    bool foundTest = false;
    for (const auto& t : types) {
        if (t.name == addedName) {
            foundTest = true;
            break;
        }
    }

    EXPECT_FALSE(types.empty());
    EXPECT_TRUE(foundTest);
}

// 4. RemoveFuseType_DeletesRow
TEST_F(FuseTypeManagerTest, RemoveFuseType_DeletesRow) {
    ASSERT_TRUE(typeMgr.add(FuseType("TestFuseType_Delete"), res)) << res.toString();

    int id = typeMgr.getByName("TestFuseType_Delete", res);
    ASSERT_GT(id, 0);

    ASSERT_TRUE(typeMgr.remove(id, res)) << res.toString();

    FuseType fetched;
    EXPECT_FALSE(typeMgr.getById(id, fetched, res));
}
