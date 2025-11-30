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

        // Seed a default category
        Category cat("TestCategory", "Category for unit tests");
        if (catMgr.addCategory(cat, res)) {
            catId = db.lastInsertId();
        }
        else {
            std::vector<Category> cats;
            ASSERT_TRUE(catMgr.listCategories(cats, res));
            catId = cats.front().id;
        }

        // Seed a default manufacturer
        Manufacturer man("TestManufacturer", "USA", "http://example.com", "For unit tests");
        if (manMgr.addManufacturer(man, res)) {
            manId = db.lastInsertId();
        }
        else {
            std::vector<Manufacturer> mans;
            ASSERT_TRUE(manMgr.listManufacturers(mans, res));
            manId = mans.front().id;
        }

        // Seed transistor lookup tables
        TransistorTypeManager typeMgr(db);
        typeMgr.addType(TransistorType("BJT"), res);

        TransistorPolarityManager polMgr(db);
        polMgr.addPolarity(TransistorPolarity("NPN"), res);

        TransistorPackageManager pkgMgr(db);
        pkgMgr.addPackage(TransistorPackage("TO-92"), res);

        // Seed resistor lookup tables
        ResistorPackageManager resPkgMgr(db);
        resPkgMgr.addPackage(ResistorPackage("0805"), res);

        ResistorCompositionManager resCompoMgr(db);
        resCompoMgr.addComposition(ResistorComposition("Carbon Film"), res);

        CapacitorPackageManager capPkgMgr(db);
        capPkgMgr.addPackage(CapacitorPackage("Radial leaded"), res);

        CapacitorDielectricManager capDielMgr(db);
        capDielMgr.addDielectric(CapacitorDielectric("C0G/NP0"), res);

    }
};
