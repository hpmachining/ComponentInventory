#pragma once

#include "IComponentEditor.h"
#include "ResistorManager.h"
#include "DbResult.h"
#include "InventoryService.h"
#include <vector>
#include <QWidget>

namespace Ui {
    class ResistorEditor;
}

class ResistorEditor : public QWidget, public IComponentEditor {
    Q_OBJECT

public:
    explicit ResistorEditor(InventoryService& inventory, QWidget* parent = nullptr);
    ~ResistorEditor() override;

    QWidget* widget() override { return this; }
    void load(int componentId) override;
	bool collect(int componentId, DbResult& result) override;
	const Resistor& resistor() const { return resistor_; }

private:
    void populateLookups();
    InventoryService& inventory_;
    Ui::ResistorEditor* ui_;
    Resistor resistor_;
};
