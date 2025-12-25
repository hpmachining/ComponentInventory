#include "BackendTestFixture.h"
#include "CategoryManager.h"
#include "LookupItem.h"

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

// 5. ListLookup_ReturnsAllCategories
TEST_F(CategoryManagerTest, ListLookup_ReturnsAllCategories) {
    ASSERT_TRUE(catMgr.addByName("CatA", res)) << res.toString();
    ASSERT_TRUE(catMgr.addByName("CatB", res)) << res.toString();

    std::vector<LookupItem> items;
    ASSERT_TRUE(catMgr.listLookup(items, res)) << res.toString();

    bool foundA = false, foundB = false;
    for (const auto& i : items) {
        if (i.name == "CatA") foundA = true;
        if (i.name == "CatB") foundB = true;
    }

    EXPECT_TRUE(foundA);
    EXPECT_TRUE(foundB);
}

// 6. AddByName_ValidatesInput
TEST_F(CategoryManagerTest, AddByName_ValidatesInput) {
    // Normal add
    ASSERT_TRUE(catMgr.addByName("NewCat", res)) << res.toString();

    // Duplicate add
    ASSERT_FALSE(catMgr.addByName("NewCat", res));
    EXPECT_TRUE(res.hasError());

    // Empty name
    ASSERT_FALSE(catMgr.addByName("", res));
    EXPECT_TRUE(res.hasError());

    // Name with whitespace only
    ASSERT_FALSE(catMgr.addByName("   ", res));
    EXPECT_TRUE(res.hasError());

    // Leading/trailing whitespace trimmed
    ASSERT_TRUE(catMgr.addByName("  TrimCat  ", res)) << res.toString();

    // Verify trimmed name exists
    int id = catMgr.getByName("TrimCat", res);
    EXPECT_GT(id, 0);
}

// 7. AddByName_IsCaseInsensitive
TEST_F(CategoryManagerTest, AddByName_IsCaseInsensitive) {
    ASSERT_TRUE(catMgr.addByName("CaseTest", res)) << res.toString();

    EXPECT_FALSE(catMgr.addByName("casetest", res));
    EXPECT_EQ(res.code, static_cast<int>(LookupError::AlreadyExists));
}
