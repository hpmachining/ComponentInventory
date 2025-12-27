#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>

struct Resistor {
    int componentId{ 0 };           // FK -> Components
    double resistance{ 0.0 };       // Ohms
    double tolerance{ 0.0 };        // Percent
    double powerRating{ 0.0 };      // Watts
    double tempCoefficient{ 0.0 };  // ppm/°C
    int packageTypeId{ 0 };         // FK -> ResistorPackage
    int compositionId{ 0 };         // FK -> ResistorComposition
    double leadSpacing{ 0.0 };      // mm
    double voltageRating{ 0.0 };    // Volts
};

class ResistorManager {
public:
    ResistorManager(Database& db) : db_(db) {}

    bool add(const Resistor& r, DbResult& result);
    bool getByComponentId(int compId, Resistor& r, DbResult& result);
    bool update(const Resistor& r, DbResult& result);
    bool remove(int compId, DbResult& result);
    bool list(std::vector<Resistor>& resistors, DbResult& result);

private:
    Database& db_;
};
