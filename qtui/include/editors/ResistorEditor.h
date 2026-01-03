#pragma once

#include <QWidget>
#include "IComponentEditor.h"
#include "ResistorManager.h"
#include "DbResult.h"
#include "InventoryService.h"   // <-- need this
#include <vector>

namespace Ui {
    class ResistorEditor;
}

class ResistorEditor : public QWidget, public IComponentEditor {
    Q_OBJECT

public:
    // now takes InventoryService& (not Database&)
    explicit ResistorEditor(InventoryService& inventory, QWidget* parent = nullptr);
    ~ResistorEditor() override;

    QWidget* widget() override { return this; }
    void load(int componentId) override;
    bool save(int componentId, DbResult& result) override;

private:
    void populateLookups();

    InventoryService& inventory_;
    Ui::ResistorEditor* ui_;
};
