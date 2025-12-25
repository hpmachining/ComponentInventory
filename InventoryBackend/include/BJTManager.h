#pragma once
#include "Database.h"
#include "DbResult.h"
#include "LookupItem.h"
#include <vector>

struct BJT {
    int componentId;
    double vceMax;
    double icMax;
    double pdMax;
    double hfe;
    double ft;

    BJT() : componentId(0), vceMax(0), icMax(0), pdMax(0), hfe(0), ft(0) {}
    BJT(int componentId_, double vceMax_, double icMax_, double pdMax_, double hfe_, double ft_)
        : componentId(componentId_), vceMax(vceMax_), icMax(icMax_), pdMax(pdMax_), hfe(hfe_), ft(ft_) {
    }
};

class BJTManager {
public:
    explicit BJTManager(Database& db);

    bool add(const BJT& bjt, DbResult& result);
    bool getById(int componentId, BJT& bjt, DbResult& result);
    bool update(const BJT& bjt, DbResult& result);
    bool remove(int componentId, DbResult& result);
    bool list(std::vector<BJT>& bjts, DbResult& result);
    bool listLookup(std::vector<LookupItem>& items, DbResult& result);

private:
    Database& db_;
};
