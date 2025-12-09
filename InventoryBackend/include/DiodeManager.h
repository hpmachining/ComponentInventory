#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>

struct Diode {
    int componentId{ 0 };
    int packageId{ 0 };
    int typeId{ 0 };
    int polarityId{ 0 };
    double forwardVoltage{ 0.0 };     // volts
    double maxCurrent{ 0.0 };         // amps
    double maxReverseVoltage{ 0.0 };  // volts
    double reverseLeakage{ 0.0 };     // µA or mA
};

class DiodeManager {
public:
    explicit DiodeManager(Database& db);

    bool addDiode(const Diode& d, DbResult& res);
    bool getDiodeByComponentId(int componentId, Diode& d, DbResult& res);
    bool updateDiode(const Diode& d, DbResult& res);
    bool deleteDiode(int componentId, DbResult& res);
    bool listDiodes(std::vector<Diode>& ds, DbResult& res);

    // Lookup helpers
    int getPackageByName(const std::string& name, DbResult& res);
    int getTypeByName(const std::string& name, DbResult& res);
    int getPolarityByName(const std::string& name, DbResult& res);

private:
    Database& db_;
    int resolveIdByName(const std::string& table, const std::string& name, DbResult& res);
};
