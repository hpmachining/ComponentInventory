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

    bool add(const TransistorPackage& pkg, DbResult& result);
    bool getById(int id, TransistorPackage& pkg, DbResult& result);
    bool list(std::vector<TransistorPackage>& pkgs, DbResult& result);
    bool remove(int id, DbResult& result);
    int getByName(const std::string& name, DbResult& result);

private:
    Database& db_;
};
