#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>

struct DiodePackage {
    int id{ 0 };
    std::string name;
    DiodePackage() = default;
    explicit DiodePackage(const std::string& n) : name(n) {}
};

class DiodePackageManager {
public:
    explicit DiodePackageManager(Database& db);

    bool addPackage(const DiodePackage& pkg, DbResult& res);
    bool getPackageById(int id, DiodePackage& pkg, DbResult& res);
    int  getByName(const std::string& name, DbResult& res);
    bool listPackages(std::vector<DiodePackage>& pkgs, DbResult& res);
    bool deletePackage(int id, DbResult& res);

private:
    Database& db_;
};
