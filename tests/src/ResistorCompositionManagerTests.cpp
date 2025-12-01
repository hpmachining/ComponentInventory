#include "BackendTestFixture.h"
#include "ResistorCompositionManager.h"

class ResistorCompositionManagerTest : public BackendTestFixture {
protected:
    ResistorCompositionManager compMgr;

    ResistorCompositionManagerTest()
        : compMgr(db) {
    }
};

// 1. AddComposition_InsertsRow
TEST_F(ResistorCompositionManagerTest, AddComposition_InsertsRow) {
    // Use a unique test-only name to avoid collision with seeded compositions
    ResistorComposition comp("TestComp_Add");
    ASSERT_TRUE(compMgr.addComposition(comp, res)) << res.toString();

    int id = compMgr.getByName("TestComp_Add", res);
    ASSERT_GT(id, 0);

    ResistorComposition fetched;
    ASSERT_TRUE(compMgr.getCompositionById(id, fetched, res)) << res.toString();
    EXPECT_EQ(fetched.id, id);
    EXPECT_EQ(fetched.name, "TestComp_Add");
}

// 2. GetCompositionById_ReturnsCorrectComposition
TEST_F(ResistorCompositionManagerTest, GetCompositionById_ReturnsCorrectComposition) {
    ResistorComposition comp("TestComp_Get");
    ASSERT_TRUE(compMgr.addComposition(comp, res)) << res.toString();

    int id = compMgr.getByName("TestComp_Get", res);
    ASSERT_GT(id, 0);

    ResistorComposition fetched;
    ASSERT_TRUE(compMgr.getCompositionById(id, fetched, res)) << res.toString();
    EXPECT_EQ(fetched.name, "TestComp_Get");
}

// 3. ListCompositions_ReturnsAllCompositions
TEST_F(ResistorCompositionManagerTest, ListCompositions_ReturnsAllCompositions) {
    // Add a unique composition
    ASSERT_TRUE(compMgr.addComposition(ResistorComposition("TestComp_List"), res)) << res.toString();

    std::vector<ResistorComposition> comps;
    ASSERT_TRUE(compMgr.listCompositions(comps, res)) << res.toString();
    ASSERT_GE(comps.size(), 2);

    bool hasCarbon = false, hasTestComp = false;
    for (const auto& c : comps) {
        if (c.name == "Carbon Film") hasCarbon = true;       // seeded canonical
        if (c.name == "TestComp_List") hasTestComp = true;   // test-only
    }
    EXPECT_TRUE(hasCarbon);
    EXPECT_TRUE(hasTestComp);
}

// 4. DeleteComposition_RemovesRow
TEST_F(ResistorCompositionManagerTest, DeleteComposition_RemovesRow) {
    ResistorComposition comp("TestComp_Delete");
    ASSERT_TRUE(compMgr.addComposition(comp, res)) << res.toString();

    int id = compMgr.getByName("TestComp_Delete", res);
    ASSERT_GT(id, 0);

    ASSERT_TRUE(compMgr.deleteComposition(id, res)) << res.toString();

    ResistorComposition fetched;
    EXPECT_FALSE(compMgr.getCompositionById(id, fetched, res));
}
