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
    // Use a distinct name to avoid collision with fixture's "Carbon Film"
    ResistorComposition comp("Metal Film");
    ASSERT_TRUE(compMgr.addComposition(comp, res)) << res.toString();

    int id = compMgr.getByName("Metal Film", res);
    ASSERT_GT(id, 0);

    ResistorComposition fetched;
    ASSERT_TRUE(compMgr.getCompositionById(id, fetched, res)) << res.toString();
    EXPECT_EQ(fetched.id, id);
    EXPECT_EQ(fetched.name, "Metal Film");
}

// 2. GetCompositionById_ReturnsCorrectComposition
TEST_F(ResistorCompositionManagerTest, GetCompositionById_ReturnsCorrectComposition) {
    ResistorComposition comp("Wirewound");
    ASSERT_TRUE(compMgr.addComposition(comp, res)) << res.toString();

    int id = compMgr.getByName("Wirewound", res);
    ASSERT_GT(id, 0);

    ResistorComposition fetched;
    ASSERT_TRUE(compMgr.getCompositionById(id, fetched, res)) << res.toString();
    EXPECT_EQ(fetched.name, "Wirewound");
}

// 3. ListCompositions_ReturnsAllCompositions
TEST_F(ResistorCompositionManagerTest, ListCompositions_ReturnsAllCompositions) {
    ResistorComposition comp("Thick Film");
    ASSERT_TRUE(compMgr.addComposition(comp, res)) << res.toString();

    std::vector<ResistorComposition> comps;
    ASSERT_TRUE(compMgr.listCompositions(comps, res)) << res.toString();
    ASSERT_GE(comps.size(), 2);

    bool hasCarbon = false, hasThick = false;
    for (const auto& c : comps) {
        if (c.name == "Carbon Film") hasCarbon = true;
        if (c.name == "Thick Film") hasThick = true;
    }
    EXPECT_TRUE(hasCarbon);
    EXPECT_TRUE(hasThick);
}

// 4. DeleteComposition_RemovesRow
TEST_F(ResistorCompositionManagerTest, DeleteComposition_RemovesRow) {
    ResistorComposition comp("Foil");
    ASSERT_TRUE(compMgr.addComposition(comp, res)) << res.toString();

    int id = compMgr.getByName("Foil", res);
    ASSERT_GT(id, 0);

    ASSERT_TRUE(compMgr.deleteComposition(id, res)) << res.toString();

    ResistorComposition fetched;
    EXPECT_FALSE(compMgr.getCompositionById(id, fetched, res));
}
