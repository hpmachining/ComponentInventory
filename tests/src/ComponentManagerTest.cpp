#include "BackendTestFixture.h"
#include "ComponentManager.h"

class ComponentManagerTest : public BackendTestFixture {
protected:
    ComponentManager compMgr;
    ComponentManagerTest() : compMgr(db) {}
};

TEST_F(ComponentManagerTest, AddAndFetchComponent) {
    Component comp("PN123", "Test component", catId, manId, 10, "Notes");
    ASSERT_TRUE(compMgr.addComponent(comp, res));

    int compId = db.lastInsertId();
    Component fetched;
    ASSERT_TRUE(compMgr.getComponentById(compId, fetched, res));
    EXPECT_EQ(fetched.partNumber, "PN123");
    EXPECT_EQ(fetched.categoryId, catId);
    EXPECT_EQ(fetched.manufacturerId, manId);
}
