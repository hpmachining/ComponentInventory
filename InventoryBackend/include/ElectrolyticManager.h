#pragma once

#include "Database.h"
#include "DbResult.h"
#include <vector>

// Electrolytic subtype struct
struct Electrolytic {
    int componentId;          // FK -> Capacitors.ComponentID
    double diameter;          // mm
    double height;            // mm
    double leadSpacing;       // mm

    Electrolytic()
        : componentId(0), diameter(0.0), height(0.0), leadSpacing(0.0) {
    }
};

// Manager class for Electrolytics
class ElectrolyticManager {
public:
    explicit ElectrolyticManager(Database& db);

    bool addElectrolytic(const Electrolytic& ecap, DbResult& result);
    bool getElectrolyticById(int id, Electrolytic& ecap, DbResult& result);
    bool updateElectrolytic(const Electrolytic& ecap, DbResult& result);
    bool deleteElectrolytic(int id, DbResult& result);
    bool listElectrolytics(std::vector<Electrolytic>& ecaps, DbResult& result);

private:
    Database& db_;
};
