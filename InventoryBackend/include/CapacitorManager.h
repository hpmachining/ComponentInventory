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
};

// Manager class for Capacitors
class CapacitorManager {
public:
    explicit CapacitorManager(Database& db);

    bool addCapacitor(const Capacitor& cap, DbResult& result);
    bool getCapacitorById(int id, Capacitor& cap, DbResult& result);
	bool getCapacitorByComponentId(int componentId, Capacitor& cap, DbResult& result);
    bool updateCapacitor(const Capacitor& cap, DbResult& result);
    bool deleteCapacitor(int id, DbResult& result);
    bool listCapacitors(std::vector<Capacitor>& caps, DbResult& result);

private:
    Database& db_;
};
