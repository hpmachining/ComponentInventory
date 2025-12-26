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
    explicit CapacitorPackage(const std::string& n) : id(0), name(n) {}
};

class CapacitorPackageManager {
public:
    explicit CapacitorPackageManager(Database& db);

    bool add(const CapacitorPackage& pkg, DbResult& result);
    bool getById(int id, CapacitorPackage& pkg, DbResult& result);
    bool list(std::vector<CapacitorPackage>& pkgs, DbResult& result);
    bool remove(int id, DbResult& result);
    int getByName(const std::string& name, DbResult& result);

private:
    Database& db_;
};
