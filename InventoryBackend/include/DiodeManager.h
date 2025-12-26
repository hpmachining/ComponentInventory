#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>

struct Diode {
    int componentId;
    int packageId;
    int typeId;
    int polarityId;
    double forwardVoltage;
    double maxCurrent;
    double maxReverseVoltage;
    double reverseLeakage;

    // Default constructor
    Diode()
        : componentId(0), packageId(0), typeId(0), polarityId(0),
        forwardVoltage(0.0), maxCurrent(0.0),
        maxReverseVoltage(0.0), reverseLeakage(0.0) {
    }

    // Parameterized constructor
    Diode(int compId, int pkgId, int typeId, int polId,
        double fwdV, double maxI, double maxRevV, double revLeak)
        : componentId(compId), packageId(pkgId),
        typeId(typeId), polarityId(polId),
        forwardVoltage(fwdV), maxCurrent(maxI),
        maxReverseVoltage(maxRevV), reverseLeakage(revLeak) {
    }
};

class DiodeManager {
public:
    explicit DiodeManager(Database& db);

    // CRUD operations
    bool add(const Diode& d, DbResult& res);
    bool getById(int componentId, Diode& d, DbResult& res);
    bool update(const Diode& d, DbResult& res);
    bool remove(int componentId, DbResult& res);
    bool list(std::vector<Diode>& ds, DbResult& res);

private:
    Database& db_;
};
