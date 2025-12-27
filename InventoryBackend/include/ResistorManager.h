#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>
#include <string>

struct Resistor {
    int componentId;            // FK -> Components (base record)
    double resistance;          // Ohms
    double tolerance;           // Percent (e.g., 5.0 for ±5%)
    double powerRating;         // Watts
    double tempCoefficient;     // ppm/°C
    int packageTypeId;          // FK -> ResistorPackage(ID)
    int compositionId;          // FK -> ResistorComposition(ID)
    double leadSpacing;         // mm
    double voltageRating;       // Volts

    Resistor()
        : componentId(0), resistance(0.0), tolerance(0.0),
        powerRating(0.0), tempCoefficient(0.0),
        packageTypeId(0), compositionId(0),
        leadSpacing(0.0), voltageRating(0.0) {
    }
};

class ResistorManager {
public:
    ResistorManager(Database& db) : db_(db) {}

    bool addResistor(const Resistor& r, DbResult& result);
    bool getResistorByComponentId(int compId, Resistor& r, DbResult& result);
    bool updateResistor(const Resistor& r, DbResult& result);
    bool deleteResistor(int compId, DbResult& result);
    bool listResistors(std::vector<Resistor>& resistors, DbResult& result);

private:
    Database& db_;
};
