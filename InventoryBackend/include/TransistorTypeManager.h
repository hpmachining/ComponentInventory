#pragma once
#include "Database.h"
#include "DbResult.h"
#include <string>
#include <vector>

struct TransistorType {
    int id;
    std::string name;

    TransistorType() : id(0), name("") {}
    explicit TransistorType(const std::string& n) : id(0), name(n) {}
};

class TransistorTypeManager {
public:
    explicit TransistorTypeManager(Database& db);

    bool addType(const TransistorType& type, DbResult& result);
    bool getTypeById(int id, TransistorType& type, DbResult& result);
    bool listTypes(std::vector<TransistorType>& types, DbResult& result);
    bool deleteType(int id, DbResult& result);
	int getByName(const std::string& name, DbResult& result);

private:
    Database& db_;
};
