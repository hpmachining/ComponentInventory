#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>
#include <string>

struct ResistorComposition {
    int id;
    std::string name;

    ResistorComposition() : id(0) {}
    explicit ResistorComposition(const std::string& n) : id(0), name(n) {}
};

class ResistorCompositionManager {
public:
    explicit ResistorCompositionManager(Database& db) : db_(db) {}

    bool add(const ResistorComposition& comp, DbResult& result);
    bool getById(int id, ResistorComposition& comp, DbResult& result);
    bool update(const ResistorComposition& comp, DbResult& result);
    bool remove(int id, DbResult& result);
    bool list(std::vector<ResistorComposition>& comps, DbResult& result);
    int getByName(const std::string& name, DbResult& result);

private:
    Database& db_;
};
