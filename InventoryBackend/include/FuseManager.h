#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>

struct Fuse {
    int componentId;
    int packageId;
    int typeId;
    double currentRating;
    double voltageRating;

    Fuse()
        : componentId(0), packageId(0), typeId(0),
        currentRating(0.0), voltageRating(0.0) {
    }

    Fuse(int compId, int pkgId, int typeId,
        double current, double voltage)
        : componentId(compId), packageId(pkgId), typeId(typeId),
        currentRating(current), voltageRating(voltage) {
    }
};

class FuseManager {
public:
    explicit FuseManager(Database& db);

    bool add(const Fuse& fuse, DbResult& res);
    bool getById(int componentId, Fuse& fuse, DbResult& res);
    bool update(const Fuse& fuse, DbResult& res);
    bool remove(int componentId, DbResult& res);
    bool list(std::vector<Fuse>& fuses, DbResult& res);

private:
    Database& db_;
};
