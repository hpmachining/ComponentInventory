#pragma once
#include "Database.h"
#include "DbResult.h"
#include <string>
#include <vector>

struct TransistorPackage {
    int id;
    std::string name;

    TransistorPackage() : id(0), name("") {}
    explicit TransistorPackage(const std::string& n) : id(0), name(n) {}
};

class TransistorPackageManager {
public:
    explicit TransistorPackageManager(Database& db);

    bool addPackage(const TransistorPackage& pkg, DbResult& result);
    bool getPackageById(int id, TransistorPackage& pkg, DbResult& result);
    bool listPackages(std::vector<TransistorPackage>& pkgs, DbResult& result);
    bool deletePackage(int id, DbResult& result);

private:
    Database& db_;
};
