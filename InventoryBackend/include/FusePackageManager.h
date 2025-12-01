#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>

struct FusePackage {
    int id{ 0 };
    std::string name;
    FusePackage() = default;
    explicit FusePackage(const std::string& n) : name(n) {}
};

class FusePackageManager {
public:
    explicit FusePackageManager(Database& db);

    bool addPackage(const FusePackage& pkg, DbResult& res);
    bool getPackageById(int id, FusePackage& pkg, DbResult& res);
    int  getByName(const std::string& name, DbResult& res);
    bool listPackages(std::vector<FusePackage>& pkgs, DbResult& res);
    bool deletePackage(int id, DbResult& res);

private:
    Database& db_;
};
