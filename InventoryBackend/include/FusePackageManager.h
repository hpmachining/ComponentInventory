#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>
#include <string>

struct FusePackage {
    int id;
    std::string name;

    FusePackage() : id(0), name() {}
    explicit FusePackage(const std::string& n) : id(0), name(n) {}
    FusePackage(int id, const std::string& n) : id(id), name(n) {}
};

class FusePackageManager {
public:
    explicit FusePackageManager(Database& db);

    bool add(const FusePackage& pkg, DbResult& res);
    bool getById(int id, FusePackage& pkg, DbResult& res);
    int  getByName(const std::string& name, DbResult& res);
    bool list(std::vector<FusePackage>& pkgs, DbResult& res);
    bool remove(int id, DbResult& res);

private:
    Database& db_;
};
