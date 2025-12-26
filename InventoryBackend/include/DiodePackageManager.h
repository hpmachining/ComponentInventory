#pragma once

#include "Database.h"
#include "DbResult.h"
#include <vector>
#include <string>

struct DiodePackage {
    int id{ 0 };
    std::string name;

    DiodePackage() = default;
    explicit DiodePackage(const std::string& n) : id(0), name(n) {}
};

class DiodePackageManager {
public:
    explicit DiodePackageManager(Database& db);

    bool add(const DiodePackage& pkg, DbResult& res);
    bool getById(int id, DiodePackage& pkg, DbResult& res);
    int  getByName(const std::string& name, DbResult& res);
    bool list(std::vector<DiodePackage>& pkgs, DbResult& res);
    bool remove(int id, DbResult& res);

private:
    Database& db_;
};
