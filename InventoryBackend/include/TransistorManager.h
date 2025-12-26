#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>

struct Transistor {
    int componentId;
    int typeId;
    int polarityId;
    int packageId;

    Transistor()
        : componentId(0), typeId(0), polarityId(0), packageId(0) {
    }

    Transistor(int componentId_, int typeId_, int polarityId_, int packageId_)
        : componentId(componentId_),
        typeId(typeId_),
        polarityId(polarityId_),
        packageId(packageId_) {
    }
};

class TransistorManager {
public:
    explicit TransistorManager(Database& db);

    bool add(const Transistor& t, DbResult& result);
    bool getById(int componentId, Transistor& t, DbResult& result);
    bool list(std::vector<Transistor>& ts, DbResult& result);
    bool remove(int componentId, DbResult& result);

private:
    Database& db_;
};
