#pragma once

#include "Database.h"
#include "DbResult.h"
#include <vector>

// Core Capacitor struct
struct Capacitor {
    int componentId;          // FK -> Components
    double capacitance;       // Farads
    double voltageRating;     // Volts
    double tolerance;         // Percent
    double esr;               // Ohms
    double leakageCurrent;    // Amps
    bool polarized;           // Electrolytic flag
    int packageTypeId;        // FK -> CapacitorPackage
    int dielectricTypeId;     // FK -> CapacitorDielectric

    // Geometry (supports both cylindrical and rectangular)
    double diameter;          // Cylindrical
    double height;            // Shared
    double leadSpacing;       // Cylindrical
    double length;            // Rectangular
    double width;             // Rectangular

    Capacitor()
        : componentId(0), capacitance(0.0), voltageRating(0.0),
        tolerance(0.0), esr(0.0), leakageCurrent(0.0),
        polarized(false), packageTypeId(0), dielectricTypeId(0),
        diameter(0.0), height(0.0), leadSpacing(0.0),
        length(0.0), width(0.0) {
    }

    explicit Capacitor(int compId, double cap, double volt, double tol,
        double e, double leak, bool pol,
        int pkgId, int dielId,
        double diam = 0.0, double h = 0.0,
        double lead = 0.0, double len = 0.0, double w = 0.0)
        : componentId(compId), capacitance(cap), voltageRating(volt),
        tolerance(tol), esr(e), leakageCurrent(leak),
        polarized(pol), packageTypeId(pkgId), dielectricTypeId(dielId),
        diameter(diam), height(h), leadSpacing(lead),
        length(len), width(w) {
    }
};

// Manager class for Capacitors
class CapacitorManager {
public:
    explicit CapacitorManager(Database& db);

    bool add(const Capacitor& cap, DbResult& result);
    bool getById(int id, Capacitor& cap, DbResult& result);
    bool update(const Capacitor& cap, DbResult& result);
    bool remove(int id, DbResult& result);
    bool list(std::vector<Capacitor>& caps, DbResult& result);

private:
    Database& db_;
};
