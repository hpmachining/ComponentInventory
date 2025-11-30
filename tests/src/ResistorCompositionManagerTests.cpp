#include "BackendTestFixture.h"
#include "ResistorCompositionManager.h"

class ResistorCompositionManagerTest : public BackendTestFixture {
protected:
    ResistorCompositionManager compMgr;
    ResistorCompositionManagerTest() : compMgr(db) {}
};

// 1. AddComposition_InsertsRow
TEST_F(ResistorCompositionManagerTest, AddComposition_InsertsRow) {
    ResistorComposition comp("Carbon Film");
    ASSERT_TRUE(compMgr.addComposition(comp, res)) << res.toString();

    int insertedCompId = db.lastInsertId();
    ResistorComposition fetched;
    ASSERT_TRUE(compMgr.getCompositionById(insertedCompId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedCompId);
    EXPECT_EQ(fetched.name, "Carbon Film");
}

// 2. GetCompositionById_ReturnsCorrectComposition
TEST_F(ResistorCompositionManagerTest, GetCompositionById_ReturnsCorrectComposition) {
    ResistorComposition comp("Metal Film");
    ASSERT_TRUE(compMgr.addComposition(comp, res)) << res.toString();
    int insertedCompId = db.lastInsertId();

    ResistorComposition fetched;
    ASSERT_TRUE(compMgr.getCompositionById(insertedCompId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedCompId);
    EXPECT_EQ(fetched.name, "Metal Film");
}

// 3. ListCompositions_ReturnsAllCompositions
TEST_F(ResistorCompositionManagerTest, ListCompositions_ReturnsAllCompositions) {
    ASSERT_TRUE(compMgr.addComposition(ResistorComposition("Wirewound"), res)) << res.toString();
    ASSERT_TRUE(compMgr.addComposition(ResistorComposition("Thick Film"), res)) << res.toString();

    std::vector<ResistorComposition> comps;
    ASSERT_TRUE(compMgr.listCompositions(comps, res)) << res.toString();

    EXPECT_GE(comps.size(), 2);

    bool foundWirewound = false, foundThickFilm = false;
    for (const auto& c : comps) {
        if (c.name == "Wirewound") foundWirewound = true;
        if (c.name == "Thick Film") foundThickFilm = true;
    }
    EXPECT_TRUE(foundWirewound);
    EXPECT_TRUE(foundThickFilm);
}

// 4. DeleteComposition_RemovesRow
TEST_F(ResistorCompositionManagerTest, DeleteComposition_RemovesRow) {
    ResistorComposition comp("DeleteMe");
    ASSERT_TRUE(compMgr.addComposition(comp, res)) << res.toString();
    int insertedCompId = db.lastInsertId();

    ASSERT_TRUE(compMgr.deleteComposition(insertedCompId, res)) << res.toString();

    ResistorComposition fetched;
    bool gotComp = compMgr.getCompositionById(insertedCompId, fetched, res);
    EXPECT_FALSE(gotComp);  // should not exist anymore
}
