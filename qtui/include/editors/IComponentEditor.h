#pragma once

#include <QWidget>
#include "DbResult.h"

class IComponentEditor {
public:
    virtual ~IComponentEditor() = default;

    // Root widget that will be embedded in ComponentEditDialog
    virtual QWidget* widget() = 0;

    // Load data for an existing component
    virtual void load(int componentId) = 0;

    // Save data for a component (insert or update handled internally)
    virtual bool extract(int componentId, DbResult& result) = 0;
	virtual void update(int componentId) = 0;
};
