#include "Database.h"
#include "DbResult.h"
#include "SchemaManager.h"
#include "CategoryManager.h"
#include "ManufacturerManager.h"
#include "ComponentManager.h"
#include "ResistorManager.h"
#include "ResistorCompositionManager.h"
#include "ResistorPackageManager.h"
#include "ConsoleUtils.h"
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    // Force console to UTF-8 output
    configureConsoleUtf8();

    DbResult res;
    Database db("inventory.db", res);
    if (!db.isOpen()) {
        std::cerr << "Failed to open database: " << res.toString() << std::endl;
        return 1;
    }

    SchemaManager schemaMgr(db);
    if (!schemaMgr.initialize(res)) {
        std::cerr << "Failed to initialize schema: " << res.toString() << std::endl;
        return 1;
    }

    // Managers
    CategoryManager catMgr(db);
    ManufacturerManager manMgr(db);
    ComponentManager compMgr(db);
    ResistorManager resMgr(db);
    ResistorCompositionManager compoMgr(db);
    ResistorPackageManager pkgMgr(db);

    // 1. Ensure lookup values exist (insert or reuse existing)
    int catId = -1;
    Category resistorCat("Resistor", "Through-hole resistors");
    if (catMgr.addCategory(resistorCat, res)) {
        catId = db.lastInsertId();
    }
    else {
        std::vector<Category> cats;
        if (catMgr.listCategories(cats, res)) {
            auto it = std::find_if(cats.begin(), cats.end(),
                [](const Category& c) { return c.name == "Resistor"; });
            if (it != cats.end()) catId = it->id;
        }
    }

    int manId = -1;
    Manufacturer vishay("Vishay", "USA", "https://www.vishay.com", "Major resistor manufacturer");
    if (manMgr.addManufacturer(vishay, res)) {
        manId = db.lastInsertId();
    }
    else {
        std::vector<Manufacturer> mans;
        if (manMgr.listManufacturers(mans, res)) {
            auto it = std::find_if(mans.begin(), mans.end(),
                [](const Manufacturer& m) { return m.name == "Vishay"; });
            if (it != mans.end()) manId = it->id;
        }
    }

    int compoId = -1;
    ResistorComposition carbonFilm("Carbon film");
    if (compoMgr.addComposition(carbonFilm, res)) {
        compoId = db.lastInsertId();
    }
    else {
        std::vector<ResistorComposition> compos;
        if (compoMgr.listCompositions(compos, res)) {
            auto it = std::find_if(compos.begin(), compos.end(),
                [](const ResistorComposition& c) { return c.name == "Carbon film"; });
            if (it != compos.end()) compoId = it->id;
        }
    }

    int pkgId = -1;
    ResistorPackage axial("Axial");
    if (pkgMgr.addPackage(axial, res)) {
        pkgId = db.lastInsertId();
    }
    else {
        std::vector<ResistorPackage> pkgs;
        if (pkgMgr.listPackages(pkgs, res)) {
            auto it = std::find_if(pkgs.begin(), pkgs.end(),
                [](const ResistorPackage& p) { return p.name == "Axial"; });
            if (it != pkgs.end()) pkgId = it->id;
        }
    }

    // 2. Add base component (insert or skip if duplicate)
    int compId = -1;
    Component baseComp("R10K", "10kΩ resistor", catId, manId, 500, "Stock for prototypes");
    if (compMgr.addComponent(baseComp, res)) {
        compId = db.lastInsertId();
    }
    else {
        std::vector<Component> comps;
        if (compMgr.listComponents(comps, res)) {
            auto it = std::find_if(comps.begin(), comps.end(),
                [](const Component& c) { return c.partNumber == "R10K"; });
            if (it != comps.end()) compId = it->id;
        }
    }

    // 3. Add resistor details linked to component (insert or skip)
    Resistor r;
    r.componentId = compId;
    r.resistance = 10000.0;   // 10kΩ
    r.tolerance = 5.0;        // ±5%
    r.powerRating = 0.25;     // 1/4 W
    r.tempCoefficient = 100.0; // ppm/°C
    r.packageTypeId = pkgId;  // Axial
    r.compositionId = compoId; // Carbon film
    r.leadSpacing = 10.0;     // mm
    r.voltageRating = 250.0;  // Volts

    if (!resMgr.addResistor(r, res)) {
        // If already exists, just continue
        res.clear();
    }

    // 4. Fetch and display
    Resistor fetched;
    if (resMgr.getResistorByComponentId(compId, fetched, res)) {
        ResistorComposition compo;
        compoMgr.getCompositionById(fetched.compositionId, compo, res);

        ResistorPackage pkg;
        pkgMgr.getPackageById(fetched.packageTypeId, pkg, res);

        std::cout << "Resistor Report:\n"
            << "ComponentID=" << fetched.componentId
            << " Resistance=" << fetched.resistance << "Ω"
            << " Tolerance=±" << fetched.tolerance << "%"
            << " Power=" << fetched.powerRating << "W"
            << " TempCoeff=" << fetched.tempCoefficient << "ppm/°C\n"
            << "Package=" << pkg.name
            << " Composition=" << compo.name
            << " LeadSpacing=" << fetched.leadSpacing << "mm"
            << " Voltage=" << fetched.voltageRating << "V"
            << std::endl;
    }

    return 0;
}