#pragma once

#include "Database.h"
#include "DbResult.h"
#include <vector>
#include <string>

// Lookup struct
struct CapacitorPackage {
    int id;
    std::string name;

    CapacitorPackage() : id(0), name("") {}
    CapacitorPackage(const std::string& n) : id(0), name(n) {}
};

class CapacitorPackageManager {
public:
    explicit CapacitorPackageManager(Database& db);

    bool addPackage(const CapacitorPackage& pkg, DbResult& result);
    bool getPackageById(int id, CapacitorPackage& pkg, DbResult& result);
    bool listPackages(std::vector<CapacitorPackage>& pkgs, DbResult& result);
    bool deletePackage(int id, DbResult& result);
	int getByName(const std::string& name, DbResult& result);

private:
    Database& db_;
};
