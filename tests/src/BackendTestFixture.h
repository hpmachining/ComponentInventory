#pragma once

#include <gtest/gtest.h>
#include "Database.h"
#include "DbResult.h"
#include "SchemaManager.h"
#include "CategoryManager.h"
#include "ManufacturerManager.h"
#include "TransistorTypeManager.h"
#include "TransistorPolarityManager.h"
#include "TransistorPackageManager.h"
#include "ResistorPackageManager.h"
#include "ResistorCompositionManager.h"
#include "CapacitorPackageManager.h"
#include "CapacitorDielectricManager.h"

// A reusable test fixture for all manager tests
class BackendTestFixture : public ::testing::Test {
protected:
    DbResult res;
    Database db;
    SchemaManager schema;
    CategoryManager catMgr;
    ManufacturerManager manMgr;

    int catId;
    int manId;

    BackendTestFixture()
        : db(":memory:", res), schema(db), catMgr(db), manMgr(db),
        catId(-1), manId(-1) {
    }

    void SetUp() override {
        ASSERT_TRUE(db.isOpen());
        ASSERT_TRUE(schema.initialize(res));

        // Use canonical seeded values from Migration 5
        catId = catMgr.getIdByName("Resistor", res);
        ASSERT_GT(catId, 0);

        manId = manMgr.getIdByName("Generic", res);
        ASSERT_GT(manId, 0);

        // No need to seed transistor/resistor/capacitor lookups here —
        // Migration 5 already inserted BJT, NPN, TO-92, Carbon Film, Radial leaded, C0G/NP0, etc.
        // Tests can resolve IDs with getByName when needed.
    }
};
