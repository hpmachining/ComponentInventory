#pragma once
#include "Database.h"
#include "DbResult.h"
#include "ResistorPackage.h"
#include <vector>

class ResistorPackageManager {
public:
    ResistorPackageManager(Database& db) : db_(db) {}

    bool addPackage(const ResistorPackage& pkg, DbResult& result);
    bool getPackageById(int id, ResistorPackage& pkg, DbResult& result);
    bool updatePackage(const ResistorPackage& pkg, DbResult& result);
    bool deletePackage(int id, DbResult& result);
    bool listPackages(std::vector<ResistorPackage>& pkgs, DbResult& result);
	int getByName(const std::string& name, DbResult& result);

private:
    Database& db_;
};
