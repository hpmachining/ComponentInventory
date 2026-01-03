#pragma once

#include <QWidget>
#include "DbResult.h"

class IComponentEditor {
public:
    virtual ~IComponentEditor() = default;

    // Root widget that will be embedded in ComponentEditDialog
    virtual QWidget* widget() = 0;
    virtual void load(int componentId) = 0;
    virtual bool collect(int componentId, DbResult& result) = 0;
};
