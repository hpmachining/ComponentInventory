#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>
#include <string>

struct Component {
    int id;
    int categoryId;
    std::string partNumber;
    int manufacturerId;
    std::string description;
    std::string notes;
    int quantity;
    std::string datasheetLink;
    std::string createdOn;
    std::string modifiedOn;

    Component()
        : id(0), categoryId(0), manufacturerId(0), quantity(0) {
    }

    Component(const std::string& pn,
        const std::string& desc,
        int catId,
        int manId,
        int qty,
        const std::string& note = "",
        const std::string& datasheet = "")
        : id(0),
        categoryId(catId),
        partNumber(pn),
        manufacturerId(manId),
        description(desc),
        notes(note),
        quantity(qty),
        datasheetLink(datasheet) {
    }
};

class ComponentManager {
public:
    explicit ComponentManager(Database& db) : db_(db) {}

    bool add(Component& comp, DbResult& result);
    bool getById(int id, Component& comp, DbResult& result);
    bool update(const Component& comp, DbResult& result);
    bool remove(int id, DbResult& result);
    bool list(std::vector<Component>& comps, DbResult& result);

private:
    Database& db_;
};
