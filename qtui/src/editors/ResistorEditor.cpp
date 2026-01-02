#include "editors/ResistorEditor.h"
#include "ui_ResistorEditor.h"

ResistorEditor::ResistorEditor(Database& db, QWidget* parent)
    : QWidget(parent),
    db_(db),
    ui_(new Ui::ResistorEditor)
{
    ui_->setupUi(this);

    // Optional polish: disable dependent fields by default
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

void ResistorEditor::load(int componentId)
{
    ResistorManager mgr(db_);
    DbResult result;
    Resistor r;

    if (!mgr.getByComponentId(componentId, r, result))
        return;

    ui_->resistanceSpin->setValue(r.resistance);
    ui_->toleranceSpin->setValue(r.tolerance);
    ui_->powerSpin->setValue(r.powerRating);

    ui_->hasTempCoeffCheck->setChecked(r.hasTempCoeff);
    ui_->tcrMinSpin->setValue(r.tempCoeffMin);
    ui_->tcrMaxSpin->setValue(r.tempCoeffMax);

    ui_->hasTempRangeCheck->setChecked(r.hasTempRange);
    ui_->tempMinSpin->setValue(r.tempMin);
    ui_->tempMaxSpin->setValue(r.tempMax);

    ui_->tcrMinSpin->setEnabled(r.hasTempCoeff);
    ui_->tcrMaxSpin->setEnabled(r.hasTempCoeff);
    ui_->tempMinSpin->setEnabled(r.hasTempRange);
    ui_->tempMaxSpin->setEnabled(r.hasTempRange);

    // Combo boxes assume ID stored in UserRole
    ui_->packageCombo->setCurrentIndex(
        ui_->packageCombo->findData(r.packageTypeId));

    ui_->compositionCombo->setCurrentIndex(
        ui_->compositionCombo->findData(r.compositionId));

    ui_->leadSpacingSpin->setValue(r.leadSpacing);
    ui_->voltageSpin->setValue(r.voltageRating);
}

bool ResistorEditor::save(int componentId, DbResult& result)
{
    ResistorManager mgr(db_);
    Resistor r;

    r.componentId = componentId;
    r.resistance = ui_->resistanceSpin->value();
    r.tolerance = ui_->toleranceSpin->value();
    r.powerRating = ui_->powerSpin->value();

    r.hasTempCoeff = ui_->hasTempCoeffCheck->isChecked();
    r.tempCoeffMin = ui_->tcrMinSpin->value();
    r.tempCoeffMax = ui_->tcrMaxSpin->value();

    r.hasTempRange = ui_->hasTempRangeCheck->isChecked();
    r.tempMin = ui_->tempMinSpin->value();
    r.tempMax = ui_->tempMaxSpin->value();

    r.packageTypeId = ui_->packageCombo->currentData().toInt();
    r.compositionId = ui_->compositionCombo->currentData().toInt();
    r.leadSpacing = ui_->leadSpacingSpin->value();
    r.voltageRating = ui_->voltageSpin->value();

    // Determine add vs update
    Resistor existing;
    DbResult lookup;

    if (mgr.getByComponentId(componentId, existing, lookup)) {
        return mgr.update(r, result);
    }
    else {
        return mgr.add(r, result);
    }
}
