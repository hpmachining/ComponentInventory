#pragma once
#include "Database.h"
#include "DbResult.h"
#include "ResistorComposition.h"
#include <vector>

class ResistorCompositionManager {
public:
    ResistorCompositionManager(Database& db) : db_(db) {}

    bool addComposition(const ResistorComposition& comp, DbResult& result);
    bool getCompositionById(int id, ResistorComposition& comp, DbResult& result);
    bool updateComposition(const ResistorComposition& comp, DbResult& result);
    bool deleteComposition(int id, DbResult& result);
    bool listCompositions(std::vector<ResistorComposition>& comps, DbResult& result);
	int getByName(const std::string& name, DbResult& result);

private:
    Database& db_;
};
