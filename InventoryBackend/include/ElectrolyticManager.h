#pragma once

#include "Database.h"
#include "DbResult.h"
#include <vector>

// Electrolytic subtype struct
struct Electrolytic {
    int componentId;    // FK -> Capacitors.ComponentID
    double diameter;    // mm
    double height;      // mm
    double leadSpacing; // mm

    // Default constructor
    Electrolytic()
        : componentId(0), diameter(0.0), height(0.0), leadSpacing(0.0) {
    }

    // Explicit parameterized constructor
    explicit Electrolytic(int compId, double dia, double h, double lead)
        : componentId(compId), diameter(dia), height(h), leadSpacing(lead) {
    }
};


// Manager class for Electrolytics
class ElectrolyticManager {
public:
    explicit ElectrolyticManager(Database& db);

    bool add(const Electrolytic& ecap, DbResult& result);
    bool getById(int id, Electrolytic& ecap, DbResult& result);
    bool update(const Electrolytic& ecap, DbResult& result);
    bool remove(int id, DbResult& result);
    bool list(std::vector<Electrolytic>& ecaps, DbResult& result);

private:
    Database& db_;
};
