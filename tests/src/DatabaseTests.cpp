#include "BackendTestFixture.h"
#include "Database.h"

class DatabaseTest : public ::testing::Test {
protected:
    DbResult res;
};

// 1. OpenMemoryDatabase_Succeeds
TEST_F(DatabaseTest, OpenMemoryDatabase_Succeeds) {
    Database db(":memory:", res);
    EXPECT_TRUE(db.isOpen());
}

// 2. OpenInvalidPath_Fails
TEST_F(DatabaseTest, OpenInvalidPath_Fails) {
    // Try to open a nonsense path
    Database db("/invalid/path/to/db.sqlite", res);
    EXPECT_FALSE(db.isOpen());
}

// 3. LastInsertId_ReturnsCorrectValue
TEST_F(DatabaseTest, LastInsertId_ReturnsCorrectValue) {
    Database db(":memory:", res);
    ASSERT_TRUE(db.isOpen());

    // Create a simple table
    ASSERT_TRUE(db.exec("CREATE TABLE Test (ID INTEGER PRIMARY KEY AUTOINCREMENT, Name TEXT);", res));

    // Insert a row
    ASSERT_TRUE(db.exec("INSERT INTO Test (Name) VALUES ('Row1');", res));
    int id1 = db.lastInsertId();
    EXPECT_EQ(id1, 1);

    // Insert another row
    ASSERT_TRUE(db.exec("INSERT INTO Test (Name) VALUES ('Row2');", res));
    int id2 = db.lastInsertId();
    EXPECT_EQ(id2, 2);
}

// 4. ExecuteInvalidSQL_ReturnsError
TEST_F(DatabaseTest, ExecuteInvalidSQL_ReturnsError) {
    Database db(":memory:", res);
    ASSERT_TRUE(db.isOpen());

    // Try to run invalid SQL
    bool ok = db.exec("THIS IS NOT VALID SQL;", res);
    EXPECT_FALSE(ok);
    EXPECT_FALSE(res.ok());  // DbResult should indicate error
}
