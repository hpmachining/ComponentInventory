#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>

struct Fuse {
    int componentId{ 0 };
    int packageId{ 0 };
    int typeId{ 0 };
    double currentRating{ 0.0 };
    double voltageRating{ 0.0 };
};

class FuseManager {
public:
    explicit FuseManager(Database& db);

    bool addFuse(const Fuse& fuse, DbResult& res);
    bool getFuseByComponentId(int componentId, Fuse& fuse, DbResult& res);
    bool updateFuse(const Fuse& fuse, DbResult& res);
    bool deleteFuse(int componentId, DbResult& res);

    // List all fuses
    bool listFuses(std::vector<Fuse>& fuses, DbResult& res);

    // Lookup helpers
    int getPackageByName(const std::string& name, DbResult& res);
    int getTypeByName(const std::string& name, DbResult& res);

private:
    Database& db_;
    int resolveIdByName(const std::string& table, const std::string& name, DbResult& res);
};
