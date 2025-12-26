#include "BackendTestFixture.h"
#include "DiodePolarityManager.h"

class DiodePolarityManagerTest : public BackendTestFixture {
protected:
    DiodePolarityManager polMgr;

    DiodePolarityManagerTest()
        : polMgr(db) {
    }

    void SetUp() override {
        BackendTestFixture::SetUp(); // seeds lookup tables
    }
};

// 1. AddPolarity_InsertsRow
TEST_F(DiodePolarityManagerTest, AddPolarity_InsertsRow) {
    ASSERT_TRUE(polMgr.add(DiodePolarity("TestPolarity"), res))
        << res.toString();

    int id = polMgr.getByName("TestPolarity", res);
    EXPECT_GT(id, 0);
}

// 2. AddPolarity_DuplicateFails
TEST_F(DiodePolarityManagerTest, AddPolarity_DuplicateFails) {
    ASSERT_TRUE(polMgr.add(DiodePolarity("DupPolarity"), res))
        << res.toString();

    bool ok = polMgr.add(DiodePolarity("DupPolarity"), res);
    EXPECT_FALSE(ok);
}

// 3. GetById_ReturnsCorrectPolarity
TEST_F(DiodePolarityManagerTest, GetById_ReturnsCorrectPolarity) {
    ASSERT_TRUE(polMgr.add(DiodePolarity("GetByIdPol"), res))
        << res.toString();

    int id = polMgr.getByName("GetByIdPol", res);
    ASSERT_GT(id, 0);

    DiodePolarity fetched;
    ASSERT_TRUE(polMgr.getById(id, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, id);
    EXPECT_EQ(fetched.name, "GetByIdPol");
}

// 4. GetById_MissingReturnsFalse
TEST_F(DiodePolarityManagerTest, GetById_MissingReturnsFalse) {
    DiodePolarity fetched;
    bool ok = polMgr.getById(999999, fetched, res);
    EXPECT_FALSE(ok);
}

// 5. GetByName_MissingReturnsMinusOne
TEST_F(DiodePolarityManagerTest, GetByName_MissingReturnsMinusOne) {
    int id = polMgr.getByName("DoesNotExist", res);
    EXPECT_EQ(id, -1);
}

// 6. ListPolarities_ReturnsAllPolarities
TEST_F(DiodePolarityManagerTest, ListPolarities_ReturnsAllPolarities) {
    const std::string addedName = "ListPolarity";
    ASSERT_TRUE(polMgr.add(DiodePolarity(addedName), res))
        << res.toString();

    std::vector<DiodePolarity> pols;
    ASSERT_TRUE(polMgr.list(pols, res)) << res.toString();

    bool foundTest = false;
    for (const auto& p : pols) {
        if (p.name == addedName) {
            foundTest = true;
            break;
        }
    }

    EXPECT_FALSE(pols.empty());
    EXPECT_TRUE(foundTest);
}

// 7. Remove_DeletesRow
TEST_F(DiodePolarityManagerTest, Remove_DeletesRow) {
    ASSERT_TRUE(polMgr.add(DiodePolarity("RemovePolarity"), res))
        << res.toString();

    int id = polMgr.getByName("RemovePolarity", res);
    ASSERT_GT(id, 0);

    ASSERT_TRUE(polMgr.remove(id, res)) << res.toString();

    DiodePolarity fetched;
    bool ok = polMgr.getById(id, fetched, res);
    EXPECT_FALSE(ok);
}
