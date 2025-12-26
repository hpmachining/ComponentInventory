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

    Capacitor()
        : componentId(0), capacitance(0.0), voltageRating(0.0),
        tolerance(0.0), esr(0.0), leakageCurrent(0.0),
        polarized(false), packageTypeId(0), dielectricTypeId(0) {
    }

    explicit Capacitor(int compId, double cap, double volt, double tol, double e, double leak,
        bool pol, int pkgId, int dielId)
        : componentId(compId), capacitance(cap), voltageRating(volt),
        tolerance(tol), esr(e), leakageCurrent(leak),
        polarized(pol), packageTypeId(pkgId), dielectricTypeId(dielId) {
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
