#pragma once

#include "IComponentEditor.h"
#include "InventoryService.h"
#include "CapacitorManager.h"
#include "DbResult.h"
#include <QWidget>

namespace Ui {
    class CapacitorEditor;
}

class CapacitorEditor : public QWidget, public IComponentEditor {
    Q_OBJECT

public:
    explicit CapacitorEditor(InventoryService& inventory, QWidget* parent = nullptr);
    ~CapacitorEditor() override;

    QWidget* widget() override { return this; }
    void load(int componentId) override;
    bool collect(int componentId, DbResult& result) override;

    const Capacitor& capacitor() const { return capacitor_; }

private slots:
    void onPackageChanged(int index);

private:
    void populateLookups();
    void setGeometryModeFromPackage(const QString& pkgName);

private:
    InventoryService& inventory_;
    Ui::CapacitorEditor* ui_;
    Capacitor capacitor_;
};
