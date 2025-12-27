#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>
#include <string>

struct ResistorPackage {
    int id;
    std::string name;

    ResistorPackage() : id(0) {}
    ResistorPackage(const std::string& n) : id(0), name(n) {}
};

class ResistorPackageManager {
public:
    ResistorPackageManager(Database& db) : db_(db) {}

    bool add(const ResistorPackage& pkg, DbResult& result);
    bool getById(int id, ResistorPackage& pkg, DbResult& result);
    bool update(const ResistorPackage& pkg, DbResult& result);
    bool remove(int id, DbResult& result);
    bool list(std::vector<ResistorPackage>& pkgs, DbResult& result);
    int getByName(const std::string& name, DbResult& result);

private:
    Database& db_;
};
