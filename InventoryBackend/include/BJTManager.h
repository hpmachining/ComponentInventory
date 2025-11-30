#pragma once
#include "Database.h"
#include "DbResult.h"
#include <vector>

struct BJT {
    int componentId;
    double vceMax;
    double icMax;
    double pdMax;
    double hfe;
    double ft;

    // Default constructor
    BJT()
        : componentId(0), vceMax(0.0), icMax(0.0), pdMax(0.0), hfe(0.0), ft(0.0) {
    }

    // Convenience constructor
    BJT(int componentId_, double vceMax_, double icMax_, double pdMax_, double hfe_, double ft_)
        : componentId(componentId_), vceMax(vceMax_), icMax(icMax_), pdMax(pdMax_), hfe(hfe_), ft(ft_) {
    }
};

class BJTManager {
public:
    explicit BJTManager(Database& db);

    bool addBJT(const BJT& bjt, DbResult& result);
    bool getBJTById(int componentId, BJT& bjt, DbResult& result);
    bool updateBJT(const BJT& bjt, DbResult& result);
    bool deleteBJT(int componentId, DbResult& result);
    bool listBJTs(std::vector<BJT>& bjts, DbResult& result);

private:
    Database& db_;
};
