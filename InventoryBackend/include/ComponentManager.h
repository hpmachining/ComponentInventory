#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>
#include <string>

struct Component {
    int id;                  // Primary key from Components table
    int categoryId;          // FK -> Categories
    std::string partNumber;  // Internal or manufacturer PN
    int manufacturerId;      // FK -> Manufacturers
    std::string description; // General description
    std::string notes;       // Free-form notes
    int quantity;            // Quantity on hand
    std::string datasheetLink; // URL or file path to datasheet
    std::string createdOn;   // Timestamp when record created
    std::string modifiedOn;  // Timestamp when record last updated

    Component()
        : id(0), categoryId(0), manufacturerId(0),
        quantity(0) {
    }

    Component(const std::string& pn,
        const std::string& desc,
        int catId,
        int manId,
        int qty,
        const std::string& note = "",
        const std::string& datasheet = "")
        : id(0), categoryId(catId), partNumber(pn),
        manufacturerId(manId), description(desc),
        notes(note), quantity(qty),
        datasheetLink(datasheet) {
    }
};

class ComponentManager {
public:
    ComponentManager(Database& db) : db_(db) {}

    bool addComponent(Component& comp, DbResult& result);
    bool getComponentById(int id, Component& comp, DbResult& result);
    bool updateComponent(const Component& comp, DbResult& result);
    bool deleteComponent(int id, DbResult& result);
    bool listComponents(std::vector<Component>& comps, DbResult& result);
	int getByPartNumber(const std::string& partNumber, DbResult& result);

private:
    Database& db_;
};
