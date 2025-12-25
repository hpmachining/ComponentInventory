#include "BackendTestFixture.h"
#include "ManufacturerManager.h"
#include "LookupItem.h"

class ManufacturerManagerTest : public BackendTestFixture {
protected:
    ManufacturerManager manMgr;
    ManufacturerManagerTest() : manMgr(db) {}
};

// 1. AddManufacturer_InsertsRow
TEST_F(ManufacturerManagerTest, AddManufacturer_InsertsRow) {
    Manufacturer man("AcmeCorp", "USA", "http://acme.example.com", "Test manufacturer");
    ASSERT_TRUE(manMgr.addManufacturer(man, res)) << res.toString();

    int insertedManId = manMgr.getByName("AcmeCorp", res);
    ASSERT_GT(insertedManId, 0);

    Manufacturer fetched;
    ASSERT_TRUE(manMgr.getManufacturerById(insertedManId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedManId);
    EXPECT_EQ(fetched.name, "AcmeCorp");
    EXPECT_EQ(fetched.country, "USA");
    EXPECT_EQ(fetched.website, "http://acme.example.com");
    EXPECT_EQ(fetched.notes, "Test manufacturer");
}

// 2. GetManufacturerById_ReturnsCorrectManufacturer
TEST_F(ManufacturerManagerTest, GetManufacturerById_ReturnsCorrectManufacturer) {
    Manufacturer man("Globex", "Germany", "http://globex.example.com", "Another test manufacturer");
    ASSERT_TRUE(manMgr.addManufacturer(man, res)) << res.toString();

    int insertedManId = manMgr.getByName("Globex", res);
    ASSERT_GT(insertedManId, 0);

    Manufacturer fetched;
    ASSERT_TRUE(manMgr.getManufacturerById(insertedManId, fetched, res)) << res.toString();

    EXPECT_EQ(fetched.id, insertedManId);
    EXPECT_EQ(fetched.name, "Globex");
    EXPECT_EQ(fetched.country, "Germany");
    EXPECT_EQ(fetched.website, "http://globex.example.com");
    EXPECT_EQ(fetched.notes, "Another test manufacturer");
}

// 3. ListManufacturers_ReturnsAllManufacturers
TEST_F(ManufacturerManagerTest, ListManufacturers_ReturnsAllManufacturers) {
    ASSERT_TRUE(manMgr.addManufacturer(Manufacturer("FooInc", "Canada", "http://foo.example.com", "Test A"), res)) << res.toString();
    ASSERT_TRUE(manMgr.addManufacturer(Manufacturer("BarLtd", "UK", "http://bar.example.com", "Test B"), res)) << res.toString();

    std::vector<Manufacturer> mans;
    ASSERT_TRUE(manMgr.listManufacturers(mans, res)) << res.toString();

    EXPECT_GE(mans.size(), 2);

    bool foundFoo = false, foundBar = false;
    for (const auto& m : mans) {
        if (m.name == "FooInc") foundFoo = true;
        if (m.name == "BarLtd") foundBar = true;
    }
    EXPECT_TRUE(foundFoo);
    EXPECT_TRUE(foundBar);
}

// 4. DeleteManufacturer_RemovesRow
TEST_F(ManufacturerManagerTest, DeleteManufacturer_RemovesRow) {
    Manufacturer man("DeleteMe", "France", "http://deleteme.example.com", "To be deleted");
    ASSERT_TRUE(manMgr.addManufacturer(man, res)) << res.toString();

    int insertedManId = manMgr.getByName("DeleteMe", res);
    ASSERT_GT(insertedManId, 0);

    ASSERT_TRUE(manMgr.deleteManufacturer(insertedManId, res)) << res.toString();

    Manufacturer fetched;
    bool gotMan = manMgr.getManufacturerById(insertedManId, fetched, res);
    EXPECT_FALSE(gotMan);  // should not exist anymore
}

// 5. ListLookup_ReturnsAllManufacturers
TEST_F(ManufacturerManagerTest, ListLookup_ReturnsAllManufacturers) {
    ASSERT_TRUE(manMgr.addByName("ManA", res)) << res.toString();
    ASSERT_TRUE(manMgr.addByName("ManB", res)) << res.toString();

    std::vector<LookupItem> items;
    ASSERT_TRUE(manMgr.listLookup(items, res)) << res.toString();

    bool foundA = false, foundB = false;
    for (const auto& i : items) {
        if (i.name == "ManA") foundA = true;
        if (i.name == "ManB") foundB = true;
    }

    EXPECT_TRUE(foundA);
    EXPECT_TRUE(foundB);
}

// 6. AddByName_ValidatesInput
TEST_F(ManufacturerManagerTest, AddByName_ValidatesInput) {
    // Normal add
    ASSERT_TRUE(manMgr.addByName("NewMan", res)) << res.toString();

    // Duplicate add
    ASSERT_FALSE(manMgr.addByName("NewMan", res));
    EXPECT_TRUE(res.hasError());

    // Empty name
    ASSERT_FALSE(manMgr.addByName("", res));
    EXPECT_TRUE(res.hasError());

    // Name with whitespace only
    ASSERT_FALSE(manMgr.addByName("   ", res));
    EXPECT_TRUE(res.hasError());

    // Leading/trailing whitespace trimmed
    ASSERT_TRUE(manMgr.addByName("  TrimMan  ", res)) << res.toString();

    // Verify trimmed name exists
    int id = manMgr.getByName("TrimMan", res);
    EXPECT_GT(id, 0);
}

// 7. AddByName_IsCaseInsensitive
TEST_F(ManufacturerManagerTest, AddByName_IsCaseInsensitive) {
    ASSERT_TRUE(manMgr.addByName("CaseTest", res)) << res.toString();

    EXPECT_FALSE(manMgr.addByName("casetest", res));
    EXPECT_EQ(res.code, static_cast<int>(LookupError::AlreadyExists));
}
