#include "BackendTestFixture.h"
#include "CategoryManager.h"

class CategoryManagerTest : public BackendTestFixture {
protected:
    CategoryManager catMgr;
    CategoryManagerTest() : catMgr(db) {}
};

// 1. AddCategory_InsertsRow
TEST_F(CategoryManagerTest, AddCategory_InsertsRow) {
    Category cat("Resistor", "Test category");
    ASSERT_TRUE(catMgr.addCategory(cat, res)) << res.toString();

    int insertedCatId = db.lastInsertId();
    Category fetched;
    ASSERT_TRUE(catMgr.getCategoryById(insertedCatId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedCatId);
    EXPECT_EQ(fetched.name, "Resistor");
    EXPECT_EQ(fetched.description, "Test category");
}

// 2. GetCategoryById_ReturnsCorrectCategory
TEST_F(CategoryManagerTest, GetCategoryById_ReturnsCorrectCategory) {
    Category cat("Capacitor", "Another test category");
    ASSERT_TRUE(catMgr.addCategory(cat, res)) << res.toString();
    int insertedCatId = db.lastInsertId();

    Category fetched;
    ASSERT_TRUE(catMgr.getCategoryById(insertedCatId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedCatId);
    EXPECT_EQ(fetched.name, "Capacitor");
    EXPECT_EQ(fetched.description, "Another test category");
}

// 3. ListCategories_ReturnsAllCategories
TEST_F(CategoryManagerTest, ListCategories_ReturnsAllCategories) {
    // Add two categories
    ASSERT_TRUE(catMgr.addCategory(Category("Inductor", "Test category A"), res)) << res.toString();
    ASSERT_TRUE(catMgr.addCategory(Category("Transformer", "Test category B"), res)) << res.toString();

    std::vector<Category> cats;
    ASSERT_TRUE(catMgr.listCategories(cats, res)) << res.toString();

    // Expect at least 2 categories (plus any seeded in fixture)
    EXPECT_GE(cats.size(), 2);

    // Verify names are present
    bool foundInductor = false, foundTransformer = false;
    for (const auto& c : cats) {
        if (c.name == "Inductor") foundInductor = true;
        if (c.name == "Transformer") foundTransformer = true;
    }
    EXPECT_TRUE(foundInductor);
    EXPECT_TRUE(foundTransformer);
}

// 4. DeleteCategory_RemovesRow
TEST_F(CategoryManagerTest, DeleteCategory_RemovesRow) {
    Category cat("Diode", "Category to delete");
    ASSERT_TRUE(catMgr.addCategory(cat, res)) << res.toString();
    int insertedCatId = db.lastInsertId();

    // Delete it
    ASSERT_TRUE(catMgr.deleteCategory(insertedCatId, res)) << res.toString();

    // Try to fetch again
    Category fetched;
    bool gotCategory = catMgr.getCategoryById(insertedCatId, fetched, res);
    EXPECT_FALSE(gotCategory);  // should not exist anymore
}
