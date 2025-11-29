#include "Database.h"
#include "DbResult.h"
#include "SchemaManager.h"
#include "CategoryManager.h"
#include "ManufacturerManager.h"
#include "ComponentManager.h"
#include "CapacitorManager.h"
#include "ElectrolyticManager.h"
#include "CapacitorPackageManager.h"
#include "CapacitorDielectricManager.h"
#include "ConsoleUtils.h"

#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    // Force console to UTF-8 output
    configureConsoleUtf8();

    DbResult res;
    Database db("inventory.db", res);   // two-argument constructor
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
    CapacitorManager capMgr(db);
    ElectrolyticManager ecapMgr(db);
    CapacitorPackageManager pkgMgr(db);
    CapacitorDielectricManager dielMgr(db);

    // 1. Ensure lookup values exist
    int catId = -1;
    Category capacitorCat("Capacitor", "Electrolytic capacitors");
    if (catMgr.addCategory(capacitorCat, res)) {
        catId = db.lastInsertId();
    }
    else {
        std::vector<Category> cats;
        if (catMgr.listCategories(cats, res)) {
            auto it = std::find_if(cats.begin(), cats.end(),
                [](const Category& c) { return c.name == "Capacitor"; });
            if (it != cats.end()) catId = it->id;
        }
    }

    int manId = -1;
    Manufacturer nichicon("Nichicon", "Japan", "https://www.nichicon.co.jp", "Major capacitor manufacturer");
    if (manMgr.addManufacturer(nichicon, res)) {
        manId = db.lastInsertId();
    }
    else {
        std::vector<Manufacturer> mans;
        if (manMgr.listManufacturers(mans, res)) {
            auto it = std::find_if(mans.begin(), mans.end(),
                [](const Manufacturer& m) { return m.name == "Nichicon"; });
            if (it != mans.end()) manId = it->id;
        }
    }

    int pkgId = -1;
    CapacitorPackage radial("Radial");
    if (pkgMgr.addPackage(radial, res)) {
        pkgId = db.lastInsertId();
    }
    else {
        std::vector<CapacitorPackage> pkgs;
        if (pkgMgr.listPackages(pkgs, res)) {
            auto it = std::find_if(pkgs.begin(), pkgs.end(),
                [](const CapacitorPackage& p) { return p.name == "Radial"; });
            if (it != pkgs.end()) pkgId = it->id;
        }
    }

    int dielId = -1;
    CapacitorDielectric alum("Aluminum");
    if (dielMgr.addDielectric(alum, res)) {
        dielId = db.lastInsertId();
    }
    else {
        std::vector<CapacitorDielectric> diels;
        if (dielMgr.listDielectrics(diels, res)) {
            auto it = std::find_if(diels.begin(), diels.end(),
                [](const CapacitorDielectric& d) { return d.name == "Aluminum"; });
            if (it != diels.end()) dielId = it->id;
        }
    }

    // 2. Add base component
    int compId = -1;
    Component baseComp("C47uF25V", "47µF electrolytic capacitor", catId, manId, 200, "Stock for audio repairs");
    if (compMgr.addComponent(baseComp, res)) {
        compId = db.lastInsertId();
    }
    else {
        std::vector<Component> comps;
        if (compMgr.listComponents(comps, res)) {
            auto it = std::find_if(comps.begin(), comps.end(),
                [](const Component& c) { return c.partNumber == "C47uF25V"; });
            if (it != comps.end()) compId = it->id;
        }
    }

    // 3. Add capacitor details
    Capacitor cap;
    cap.componentId = compId;
    cap.capacitance = 0.000047;   // 47 µF
    cap.voltageRating = 25.0;     // Volts
    cap.tolerance = 20.0;         // ±20%
    cap.esr = 0.1;                // Ohms
    cap.leakageCurrent = 0.00001; // Amps
    cap.polarized = true;
    cap.packageTypeId = pkgId;
    cap.dielectricTypeId = dielId;

    if (!capMgr.addCapacitor(cap, res)) {
        res.clear(); // If already exists, just continue
    }

    // 4. Add electrolytic subtype
    Electrolytic ecap;
    ecap.componentId = compId;
    ecap.diameter = 6.3;
    ecap.height = 11.0;
    ecap.leadSpacing = 2.5;

    if (!ecapMgr.addElectrolytic(ecap, res)) {
        res.clear();
    }

    // 5. Fetch and display combined report
    Capacitor fetchedCap;
    if (capMgr.getCapacitorById(compId, fetchedCap, res)) {
        CapacitorPackage pkg;
        if (!pkgMgr.getPackageById(fetchedCap.packageTypeId, pkg, res)) {
            pkg.name = "(unknown)";
            res.clear();
        }

        CapacitorDielectric diel;
        if (!dielMgr.getDielectricById(fetchedCap.dielectricTypeId, diel, res)) {
            diel.name = "(unknown)";
            res.clear();
        }

        std::cout << "Capacitor Report:\n"
            << "ComponentID=" << fetchedCap.componentId << "\n"
            << "PartNumber=" << baseComp.partNumber << "\n"
            << "Capacitance=" << fetchedCap.capacitance << " F\n"
            << "Voltage=" << fetchedCap.voltageRating << " V\n"
            << "Tolerance=±" << fetchedCap.tolerance << "%\n"
            << "ESR=" << fetchedCap.esr << " Ω\n"
            << "Leakage=" << fetchedCap.leakageCurrent << " A\n"
            << "Polarized=" << (fetchedCap.polarized ? "Yes" : "No") << "\n"
            << "Package=" << pkg.name << "\n"
            << "Dielectric=" << diel.name << "\n"
            << std::endl;
    }

    Electrolytic fetchedEcap;
    if (ecapMgr.getElectrolyticById(compId, fetchedEcap, res)) {
        std::cout << "Electrolytic Geometry:\n"
            << "Diameter=" << fetchedEcap.diameter << " mm\n"
            << "Height=" << fetchedEcap.height << " mm\n"
            << "LeadSpacing=" << fetchedEcap.leadSpacing << " mm\n"
            << std::endl;
    }

    return 0;
}
