#pragma once
#include "Database.h"
#include "DbResult.h"
#include "Component.h"
#include <vector>

class ComponentManager {
public:
    ComponentManager(Database& db) : db_(db) {}

    bool addComponent(const Component& comp, DbResult& result);
    bool getComponentById(int id, Component& comp, DbResult& result);
    bool updateComponent(const Component& comp, DbResult& result);
    bool deleteComponent(int id, DbResult& result);
    bool listComponents(std::vector<Component>& comps, DbResult& result);
	int getByPartNumber(const std::string& partNumber, DbResult& result);

private:
    Database& db_;
};
