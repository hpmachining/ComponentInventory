#pragma once
#include "Database.h"
#include "DbResult.h"
#include "Resistor.h"
#include <vector>

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
