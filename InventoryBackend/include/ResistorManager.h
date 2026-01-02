#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>

struct Resistor {
    int componentId;           // FK -> Components

    double resistance;         // Ohms
    double tolerance;          // Percent
    double powerRating;        // Watts

    // Temperature coefficient (ppm/°C)
    double tempCoeffMin;       // Minimum TCR
    double tempCoeffMax;       // Maximum TCR
    bool hasTempCoeff;         // Manufacturer specified TCR?

    // Operating temperature range (°C)
    double tempMin;            // Minimum operating temperature
    double tempMax;            // Maximum operating temperature
    bool hasTempRange;         // Manufacturer specified temp range?

    int packageTypeId;         // FK -> ResistorPackage
    int compositionId;         // FK -> ResistorComposition
    double leadSpacing;        // mm
    double voltageRating;      // Volts

    // Default constructor
    Resistor()
        : componentId(0),
        resistance(0.0),
        tolerance(0.0),
        powerRating(0.0),
        tempCoeffMin(0.0),
        tempCoeffMax(0.0),
        hasTempCoeff(false),
        tempMin(0.0),
        tempMax(0.0),
        hasTempRange(false),
        packageTypeId(0),
        compositionId(0),
        leadSpacing(0.0),
        voltageRating(0.0) {
    }

    // Parameterized constructor
    explicit Resistor(
        int compId,
        double res,
        double tol,
        double power,
        bool tcrSpecified,
        double tcrMin,
        double tcrMax,
        bool tempRangeSpecified,
        double tMin,
        double tMax,
        int pkgId,
        int compTypeId,
        double lead,
        double volt
    )
        : componentId(compId),
        resistance(res),
        tolerance(tol),
        powerRating(power),
        tempCoeffMin(tcrMin),
        tempCoeffMax(tcrMax),
        hasTempCoeff(tcrSpecified),
        tempMin(tMin),
        tempMax(tMax),
        hasTempRange(tempRangeSpecified),
        packageTypeId(pkgId),
        compositionId(compTypeId),
        leadSpacing(lead),
        voltageRating(volt) {
    }
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
