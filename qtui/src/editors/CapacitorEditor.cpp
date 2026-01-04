#include "editors/CapacitorEditor.h"
#include "CapacitorManager.h"
#include "CapacitorPackageManager.h"
#include "CapacitorDielectricManager.h"
#include "InventoryService.h"
#include "ui_CapacitorEditor.h"

#include <QComboBox>

CapacitorEditor::CapacitorEditor(InventoryService& inventory, QWidget* parent)
    : QWidget(parent),
    inventory_(inventory),
    ui_(new Ui::CapacitorEditor)
{
    ui_->setupUi(this);

    // Populate lookup combos before load()
    populateLookups();

    // Package → geometry switching
    connect(ui_->packageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &CapacitorEditor::onPackageChanged);
}

CapacitorEditor::~CapacitorEditor()
{
    delete ui_;
}

//
// Populate package + dielectric combos
//
void CapacitorEditor::populateLookups()
{
    DbResult result;

    // Packages
    ui_->packageCombo->clear();
    std::vector<CapacitorPackage> pkgs;
    if (inventory_.capacitorPackages().list(pkgs, result)) {
        for (const auto& p : pkgs)
            ui_->packageCombo->addItem(QString::fromStdString(p.name), p.id);
    }

    // Dielectrics
    ui_->dielectricCombo->clear();
    std::vector<CapacitorDielectric> dies;
    if (inventory_.capacitorDielectrics().list(dies, result)) {
        for (const auto& d : dies)
            ui_->dielectricCombo->addItem(QString::fromStdString(d.name), d.id);
    }
}

//
// Load capacitor from DB
//
void CapacitorEditor::load(int componentId)
{
    capacitor_ = Capacitor(); // reset
    DbResult result;

    // If no capacitor row exists, clear UI and return
    if (!inventory_.capacitors().getById(componentId, capacitor_, result)) {
        ui_->capacitanceSpin->setValue(0.0);
        ui_->voltageSpin->setValue(0.0);
        ui_->toleranceSpin->setValue(0.0);
        ui_->esrSpin->setValue(0.0);
        ui_->leakageSpin->setValue(0.0);
        ui_->polarizedCheck->setChecked(false);

        ui_->heightSpin->setValue(0.0);
        ui_->diameterSpin->setValue(0.0);
        ui_->leadSpacingSpin->setValue(0.0);
        ui_->lengthSpin->setValue(0.0);
        ui_->widthSpin->setValue(0.0);

        return;
    }

    // Electrical
    ui_->capacitanceSpin->setValue(capacitor_.capacitance);
    ui_->voltageSpin->setValue(capacitor_.voltageRating);
    ui_->toleranceSpin->setValue(capacitor_.tolerance);
    ui_->esrSpin->setValue(capacitor_.esr);
    ui_->leakageSpin->setValue(capacitor_.leakageCurrent);
    ui_->polarizedCheck->setChecked(capacitor_.polarized);

    // Combos
    int pkgIndex = ui_->packageCombo->findData(capacitor_.packageTypeId);
    if (pkgIndex >= 0)
        ui_->packageCombo->setCurrentIndex(pkgIndex);

    int dielIndex = ui_->dielectricCombo->findData(capacitor_.dielectricTypeId);
    if (dielIndex >= 0)
        ui_->dielectricCombo->setCurrentIndex(dielIndex);

    // Geometry mode
    QString pkgName = ui_->packageCombo->currentText();
    setGeometryModeFromPackage(pkgName);

    // Shared height
    ui_->heightSpin->setValue(capacitor_.height);

    // Radial geometry
    ui_->diameterSpin->setValue(capacitor_.diameter);
    ui_->leadSpacingSpin->setValue(capacitor_.leadSpacing);

    // SMD geometry
    ui_->lengthSpin->setValue(capacitor_.length);
    ui_->widthSpin->setValue(capacitor_.width);
}

//
// Collect UI → capacitor_
//
bool CapacitorEditor::collect(int componentId, DbResult& result)
{
    capacitor_.componentId = componentId;

    // Electrical
    capacitor_.capacitance = ui_->capacitanceSpin->value();
    capacitor_.voltageRating = ui_->voltageSpin->value();
    capacitor_.tolerance = ui_->toleranceSpin->value();
    capacitor_.esr = ui_->esrSpin->value();
    capacitor_.leakageCurrent = ui_->leakageSpin->value();
    capacitor_.polarized = ui_->polarizedCheck->isChecked();

    // Combos
    capacitor_.packageTypeId = ui_->packageCombo->currentData().toInt();
    capacitor_.dielectricTypeId = ui_->dielectricCombo->currentData().toInt();

    // Shared height
    capacitor_.height = ui_->heightSpin->value();

    // Geometry based on active page
    int page = ui_->geometryStack->currentIndex();

    if (page == 0) { // radial
        capacitor_.diameter = ui_->diameterSpin->value();
        capacitor_.leadSpacing = ui_->leadSpacingSpin->value();
        capacitor_.length = 0.0;
        capacitor_.width = 0.0;
    }
    else { // smd
        capacitor_.length = ui_->lengthSpin->value();
        capacitor_.width = ui_->widthSpin->value();
        capacitor_.diameter = 0.0;
        capacitor_.leadSpacing = 0.0;
    }

    return true;
}

//
// Package selection → geometry mode
//
void CapacitorEditor::onPackageChanged(int index)
{
    QString pkgName = ui_->packageCombo->itemText(index);
    setGeometryModeFromPackage(pkgName);
}

//
// Decide geometry mode based on package name
//
void CapacitorEditor::setGeometryModeFromPackage(const QString& pkgName)
{
    QString lower = pkgName.toLower();

    if (lower.contains("radial") || lower.contains("leaded")) {
        ui_->geometryStack->setCurrentWidget(ui_->radialPage);
    }
    else if (lower.contains("smd") || lower.contains("chip")) {
        ui_->geometryStack->setCurrentWidget(ui_->smdPage);
    }
    else {
        // Default to SMD if unknown
        ui_->geometryStack->setCurrentWidget(ui_->smdPage);
    }
}
