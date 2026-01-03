#include "editors/ResistorEditor.h"
#include "ui_ResistorEditor.h"
#include "ResistorPackageManager.h"
#include "ResistorCompositionManager.h"
//#include <QDebug>

ResistorEditor::ResistorEditor(InventoryService& inventory, QWidget* parent)
    : QWidget(parent),
    inventory_(inventory),
    ui_(new Ui::ResistorEditor)
{
    ui_->setupUi(this);

    // Populate lookup combos first so load() can set current indexes safely
    populateLookups();

    ui_->tcrMinSpin->setEnabled(false);
    ui_->tcrMaxSpin->setEnabled(false);
    ui_->tempMinSpin->setEnabled(false);
    ui_->tempMaxSpin->setEnabled(false);

    connect(ui_->hasTempCoeffCheck, &QCheckBox::toggled, this, [this](bool checked) {
        ui_->tcrMinSpin->setEnabled(checked);
        ui_->tcrMaxSpin->setEnabled(checked);
        });

    connect(ui_->hasTempRangeCheck, &QCheckBox::toggled, this, [this](bool checked) {
        ui_->tempMinSpin->setEnabled(checked);
        ui_->tempMaxSpin->setEnabled(checked);
        });
}

ResistorEditor::~ResistorEditor()
{
    delete ui_;
}

void ResistorEditor::populateLookups()
{
    DbResult result;

    // Packages
    ui_->packageCombo->clear();
    std::vector<ResistorPackage> pkgs;
    if (inventory_.resistorPackages().list(pkgs, result)) {
        for (const auto& p : pkgs)
            ui_->packageCombo->addItem(QString::fromStdString(p.name), p.id);
    }

    // Compositions
    ui_->compositionCombo->clear();
    std::vector<ResistorComposition> comps;
    if (inventory_.resistorCompositions().list(comps, result)) {
        for (const auto& c : comps)
            ui_->compositionCombo->addItem(QString::fromStdString(c.name), c.id);
    }
}

void ResistorEditor::load(int componentId)
{
    Resistor r;
    DbResult result;

    // If no resistor row exists for this component, just clear UI defaults
    if (!inventory_.resistors().getByComponentId(componentId, r, result)) {
        ui_->resistanceSpin->setValue(0.0);
        ui_->toleranceSpin->setValue(0.0);
        ui_->powerSpin->setValue(0.0);
        ui_->leadSpacingSpin->setValue(0.0);

        ui_->hasTempCoeffCheck->setChecked(false);
        ui_->tcrMinSpin->setValue(0.0);
        ui_->tcrMaxSpin->setValue(0.0);

        ui_->hasTempRangeCheck->setChecked(false);
        ui_->tempMinSpin->setValue(0.0);
        ui_->tempMaxSpin->setValue(0.0);

        return;
    }

    ui_->resistanceSpin->setValue(r.resistance);
    ui_->toleranceSpin->setValue(r.tolerance);
    ui_->powerSpin->setValue(r.powerRating);
    ui_->leadSpacingSpin->setValue(r.leadSpacing);

    ui_->hasTempCoeffCheck->setChecked(r.hasTempCoeff);
    ui_->tcrMinSpin->setValue(r.tempCoeffMin);
    ui_->tcrMaxSpin->setValue(r.tempCoeffMax);
    ui_->tcrMinSpin->setEnabled(r.hasTempCoeff);
    ui_->tcrMaxSpin->setEnabled(r.hasTempCoeff);

    ui_->hasTempRangeCheck->setChecked(r.hasTempRange);
    ui_->tempMinSpin->setValue(r.tempMin);
    ui_->tempMaxSpin->setValue(r.tempMax);
    ui_->tempMinSpin->setEnabled(r.hasTempRange);
    ui_->tempMaxSpin->setEnabled(r.hasTempRange);

    // Set package / composition combos
    int pkgIndex = ui_->packageCombo->findData(r.packageTypeId);
    if (pkgIndex >= 0)
        ui_->packageCombo->setCurrentIndex(pkgIndex);

    int compIndex = ui_->compositionCombo->findData(r.compositionId);
    if (compIndex >= 0)
        ui_->compositionCombo->setCurrentIndex(compIndex);
}

bool ResistorEditor::collect(int componentId, DbResult& result)
{
    Resistor r;
    r.componentId = componentId;

    r.resistance = ui_->resistanceSpin->value();
    r.tolerance = ui_->toleranceSpin->value();
    r.powerRating = ui_->powerSpin->value();
    r.leadSpacing = ui_->leadSpacingSpin->value();

    r.hasTempCoeff = ui_->hasTempCoeffCheck->isChecked();
    if (r.hasTempCoeff) {
        r.tempCoeffMin = ui_->tcrMinSpin->value();
        r.tempCoeffMax = ui_->tcrMaxSpin->value();
    }

    r.hasTempRange = ui_->hasTempRangeCheck->isChecked();
    if (r.hasTempRange) {
        r.tempMin = ui_->tempMinSpin->value();
        r.tempMax = ui_->tempMaxSpin->value();
    }

    r.packageTypeId = ui_->packageCombo->currentData().toInt();
    r.compositionId = ui_->compositionCombo->currentData().toInt();

    resistor_ = r;   // store internally
    return true;
}
