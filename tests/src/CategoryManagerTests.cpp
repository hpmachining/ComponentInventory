#include "BackendTestFixture.h"
#include "CategoryManager.h"

class CategoryManagerTest : public BackendTestFixture {
protected:
    CategoryManager catMgr;
    CategoryManagerTest() : catMgr(db) {}
};

// 1. AddCategory_InsertsRow
TEST_F(CategoryManagerTest, AddCategory_InsertsRow) {
    // Use a unique test-only name to avoid collision with seeded categories
    Category cat("TestCategory_Add", "Category for add test");
    ASSERT_TRUE(catMgr.addCategory(cat, res)) << res.toString();

    int insertedCatId = catMgr.getByName("TestCategory_Add", res);
    ASSERT_GT(insertedCatId, 0);

    Category fetched;
    ASSERT_TRUE(catMgr.getCategoryById(insertedCatId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedCatId);
    EXPECT_EQ(fetched.name, "TestCategory_Add");
    EXPECT_EQ(fetched.description, "Category for add test");
}

// 2. GetCategoryById_ReturnsCorrectCategory
TEST_F(CategoryManagerTest, GetCategoryById_ReturnsCorrectCategory) {
    // Again, use a unique name
    Category cat("TestCategory_Get", "Category for get test");
    ASSERT_TRUE(catMgr.addCategory(cat, res)) << res.toString();

    int insertedCatId = catMgr.getByName("TestCategory_Get", res);
    ASSERT_GT(insertedCatId, 0);

    Category fetched;
    ASSERT_TRUE(catMgr.getCategoryById(insertedCatId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedCatId);
    EXPECT_EQ(fetched.name, "TestCategory_Get");
    EXPECT_EQ(fetched.description, "Category for get test");
}

// 3. ListCategories_ReturnsAllCategories
TEST_F(CategoryManagerTest, ListCategories_ReturnsAllCategories) {
    // Add two unique categories
    ASSERT_TRUE(catMgr.addCategory(Category("TestCategory_ListA", "Category A"), res)) << res.toString();
    ASSERT_TRUE(catMgr.addCategory(Category("TestCategory_ListB", "Category B"), res)) << res.toString();

    std::vector<Category> cats;
    ASSERT_TRUE(catMgr.listCategories(cats, res)) << res.toString();

    bool foundA = false, foundB = false;
    for (const auto& c : cats) {
        if (c.name == "TestCategory_ListA") foundA = true;
        if (c.name == "TestCategory_ListB") foundB = true;
    }
    EXPECT_TRUE(foundA);
    EXPECT_TRUE(foundB);
}

// 4. DeleteCategory_RemovesRow
TEST_F(CategoryManagerTest, DeleteCategory_RemovesRow) {
    Category cat("TestCategory_Delete", "Category to delete");
    ASSERT_TRUE(catMgr.addCategory(cat, res)) << res.toString();

    int insertedCatId = catMgr.getByName("TestCategory_Delete", res);
    ASSERT_GT(insertedCatId, 0);

    ASSERT_TRUE(catMgr.deleteCategory(insertedCatId, res)) << res.toString();

    Category fetched;
    bool gotCategory = catMgr.getCategoryById(insertedCatId, fetched, res);
    EXPECT_FALSE(gotCategory);
}
