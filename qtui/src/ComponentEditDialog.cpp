#include "ComponentEditDialog.h"
#include "ui_ComponentEditDialog.h"
#include "InventoryService.h"
#include "AddLookupDialog.h"
#include "DbResult.h"
#include "ResistorManager.h"
#include "ResistorPackageManager.h"
#include "ResistorCompositionManager.h"

#include <QMessageBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTimer>

ComponentEditDialog::ComponentEditDialog(
    InventoryService& inventory,
    QWidget* parent
)
    : QDialog(parent)
    , ui_(new Ui::ComponentEditDialog)
    , inventory_(inventory)
{
    ui_->setupUi(this);
    setWindowTitle(tr("Add Component"));

    // Make Tab move focus instead of inserting a tab
    ui_->notesEdit->setTabChangesFocus(true);

    // Start with no category-specific fields
    ui_->categoryStack->setCurrentIndex(Page_None);

    populateCombos();
    prevCategoryId_ = ui_->categoryCombo->currentData().toInt();
    prevManufacturerId_ = ui_->manufacturerCombo->currentData().toInt();

    populateResistorLookups();

    // Disable OK initially
    ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    // Live validation
    connect(ui_->partNumberEdit, &QLineEdit::textChanged,
        this, &ComponentEditDialog::updateOkButtonState);
    connect(ui_->categoryCombo, &QComboBox::currentIndexChanged,
        this, &ComponentEditDialog::updateOkButtonState);
    connect(ui_->manufacturerCombo, &QComboBox::currentIndexChanged,
        this, &ComponentEditDialog::updateOkButtonState);

    connect(ui_->categoryCombo,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &ComponentEditDialog::onCategoryChanged);

    connect(ui_->manufacturerCombo,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &ComponentEditDialog::onManufacturerChanged);

    // Temp coefficient enable
    connect(ui_->tempCoeffEnableCheck, &QCheckBox::toggled, this,
        [this](bool enabled) {
            ui_->tempCoeffMinSpin->setEnabled(enabled);
            ui_->tempCoeffMaxSpin->setEnabled(enabled);
        });

    // Temperature range enable
    connect(ui_->tempRangeEnableCheck, &QCheckBox::toggled, this,
        [this](bool enabled) {
            ui_->tempMinSpin->setEnabled(enabled);
            ui_->tempMaxSpin->setEnabled(enabled);
        });

}

ComponentEditDialog::~ComponentEditDialog()
{
    delete ui_;
}

void ComponentEditDialog::setComponent(const Component& c)
{
    component_ = c;

    ui_->partNumberEdit->setText(QString::fromStdString(c.partNumber));
    ui_->descriptionEdit->setText(QString::fromStdString(c.description));
    ui_->quantitySpin->setValue(c.quantity);
    ui_->notesEdit->setPlainText(QString::fromStdString(c.notes));
    ui_->datasheetEdit->setText(QString::fromStdString(c.datasheetLink));

    // Select category
    int catIndex = ui_->categoryCombo->findData(c.categoryId);
    if (catIndex >= 0)
        ui_->categoryCombo->setCurrentIndex(catIndex);

    // Select manufacturer
    int manIndex = ui_->manufacturerCombo->findData(c.manufacturerId);
    if (manIndex >= 0)
        ui_->manufacturerCombo->setCurrentIndex(manIndex);

    // Switch page
    int page = pageForCategory(c.categoryId);
    ui_->categoryStack->setCurrentIndex(page);

    // Load subtype fields
    if (page == Page_Resistor && c.id > 0)
        loadResistorFields(c.id);

    // Adjust dialog size after loading
    QTimer::singleShot(0, this, [this]() { adjustSize(); });
}

Component ComponentEditDialog::component() const
{
    Component c = component_;

    c.partNumber = ui_->partNumberEdit->text().toStdString();
    c.description = ui_->descriptionEdit->text().toStdString();
    c.quantity = ui_->quantitySpin->value();
    c.notes = ui_->notesEdit->toPlainText().toStdString();
    c.datasheetLink = ui_->datasheetEdit->text().toStdString();

    c.categoryId = ui_->categoryCombo->currentData().toInt();

    QVariant manData = ui_->manufacturerCombo->currentData();
    c.manufacturerId = manData.isValid() ? manData.toInt() : 0;

    return c;
}

Resistor ComponentEditDialog::resistor() const
{
    return resistor_;
}

void ComponentEditDialog::populateCombos()
{
    DbResult result;

    // Categories
    ui_->categoryCombo->blockSignals(true);
    ui_->categoryCombo->clear();

    int indexToSelect = -1;
    std::vector<LookupItem> categories;

    if (inventory_.categories().listLookup(categories, result)) {
        for (const auto& c : categories) {
            int index = ui_->categoryCombo->count();
            ui_->categoryCombo->addItem(QString::fromStdString(c.name), c.id);

            if (c.id == prevCategoryId_)
                indexToSelect = index;
        }
    }

    if (indexToSelect < 0 && !categories.empty())
        indexToSelect = 0;

    ui_->categoryCombo->setCurrentIndex(indexToSelect);
    ui_->categoryCombo->blockSignals(false);

    // Manufacturers
    ui_->manufacturerCombo->blockSignals(true);
    ui_->manufacturerCombo->clear();

    indexToSelect = -1;
    std::vector<LookupItem> manufacturers;

    if (inventory_.manufacturers().listLookup(manufacturers, result)) {
        for (const auto& m : manufacturers) {
            int index = ui_->manufacturerCombo->count();
            ui_->manufacturerCombo->addItem(QString::fromStdString(m.name), m.id);

            if (m.id == prevManufacturerId_)
                indexToSelect = index;
        }
    }

    ui_->manufacturerCombo->addItem(tr("+ Add New..."), kAddNewId);

    if (indexToSelect < 0 && !manufacturers.empty())
        indexToSelect = 0;

    ui_->manufacturerCombo->setCurrentIndex(indexToSelect);
    ui_->manufacturerCombo->blockSignals(false);
}

int ComponentEditDialog::pageForCategory(int categoryId) const
{
    int idx = ui_->categoryCombo->findData(categoryId);
    if (idx < 0)
        return Page_None;

    QString name = ui_->categoryCombo->itemText(idx);

    if (name.compare("Resistor", Qt::CaseInsensitive) == 0)
        return Page_Resistor;

    return Page_None;
}

void ComponentEditDialog::populateResistorLookups()
{
    DbResult result;

    ui_->resistorPackageCombo->clear();
    std::vector<ResistorPackage> pkgs;
    if (inventory_.resistorPackages().list(pkgs, result)) {
        for (const auto& p : pkgs)
            ui_->resistorPackageCombo->addItem(QString::fromStdString(p.name), p.id);
    }

    ui_->resistorCompositionCombo->clear();
    std::vector<ResistorComposition> comps;
    if (inventory_.resistorCompositions().list(comps, result)) {
        for (const auto& c : comps)
            ui_->resistorCompositionCombo->addItem(QString::fromStdString(c.name), c.id);
    }
}

void ComponentEditDialog::loadResistorFields(int componentId)
{
    DbResult result;
    Resistor r;

    if (!inventory_.resistors().getByComponentId(componentId, r, result))
        return;

    resistor_ = r; // keep local copy

    ui_->resistanceSpin->setValue(r.resistance);
    ui_->toleranceSpin->setValue(r.tolerance);
    ui_->powerSpin->setValue(r.powerRating);
    ui_->voltageSpin->setValue(r.voltageRating);
    ui_->leadSpacingSpin->setValue(r.leadSpacing);

    // Temp coefficient
    ui_->tempCoeffEnableCheck->setChecked(r.hasTempCoeff);
    if (r.hasTempCoeff) {
        ui_->tempCoeffMinSpin->setValue(r.tempCoeffMin);
        ui_->tempCoeffMaxSpin->setValue(r.tempCoeffMax);
    }

    // Temperature range
    ui_->tempRangeEnableCheck->setChecked(r.hasTempRange);
    if (r.hasTempRange) {
        ui_->tempMinSpin->setValue(r.tempMin);
        ui_->tempMaxSpin->setValue(r.tempMax);
    }

    populateResistorLookups();

    int pkgIndex = ui_->resistorPackageCombo->findData(r.packageTypeId);
    if (pkgIndex >= 0)
        ui_->resistorPackageCombo->setCurrentIndex(pkgIndex);

    int compIndex = ui_->resistorCompositionCombo->findData(r.compositionId);
    if (compIndex >= 0)
        ui_->resistorCompositionCombo->setCurrentIndex(compIndex);
}

void ComponentEditDialog::saveResistorFields()
{
    resistor_.resistance = ui_->resistanceSpin->value();
    resistor_.tolerance = ui_->toleranceSpin->value();
    resistor_.powerRating = ui_->powerSpin->value();
    resistor_.voltageRating = ui_->voltageSpin->value();
    resistor_.leadSpacing = ui_->leadSpacingSpin->value();
    resistor_.packageTypeId = ui_->resistorPackageCombo->currentData().toInt();
    resistor_.compositionId = ui_->resistorCompositionCombo->currentData().toInt();

    // Temp coefficient
    resistor_.hasTempCoeff = ui_->tempCoeffEnableCheck->isChecked();
    if (resistor_.hasTempCoeff) {
        resistor_.tempCoeffMin = ui_->tempCoeffMinSpin->value();
        resistor_.tempCoeffMax = ui_->tempCoeffMaxSpin->value();
    }

    // Temperature range
    resistor_.hasTempRange = ui_->tempRangeEnableCheck->isChecked();
    if (resistor_.hasTempRange) {
        resistor_.tempMin = ui_->tempMinSpin->value();
        resistor_.tempMax = ui_->tempMaxSpin->value();
    }
}

void ComponentEditDialog::accept()
{
    if (!isValid()) {
        QMessageBox::warning(this, tr("Invalid Input"),
            tr("Please enter a part number."));
        return;
    }

    // Update in-memory component and resistor
    component_ = component();      // update component_ from UI
    saveResistorFields();           // update resistor_ from UI
    QDialog::accept();
}


bool ComponentEditDialog::isValid() const
{
    return !ui_->partNumberEdit->text().trimmed().isEmpty();
}

void ComponentEditDialog::updateOkButtonState()
{
    ui_->buttonBox->button(QDialogButtonBox::Ok)
        ->setEnabled(isValid());
}

void ComponentEditDialog::onCategoryChanged(int index)
{
    if (index < 0)
        return;

    const int id = ui_->categoryCombo->itemData(index).toInt();

    if (id == kAddNewId) {
        AddLookupDialog dlg(
            tr("Add Category"),
            [&](const std::string& name, DbResult& r) {
                return inventory_.categories().addByName(name, r);
            },
            this
        );

        if (dlg.exec() == QDialog::Accepted) {
            populateCombos();
            QString added = QString::fromStdString(dlg.addedName());
            int newIndex = ui_->categoryCombo->findText(added);

            if (newIndex >= 0) {
                ui_->categoryCombo->setCurrentIndex(newIndex);
                prevCategoryId_ = ui_->categoryCombo->itemData(newIndex).toInt();
            }
            else {
                ui_->categoryCombo->setCurrentIndex(0);
                prevCategoryId_ = ui_->categoryCombo->itemData(0).toInt();
            }
        }
        else {
            int restoreIndex = ui_->categoryCombo->findData(prevCategoryId_);
            if (restoreIndex >= 0)
                ui_->categoryCombo->setCurrentIndex(restoreIndex);
        }

        int currentId = ui_->categoryCombo->currentData().toInt();
        int page = pageForCategory(currentId);
        ui_->categoryStack->setCurrentIndex(page);

        QTimer::singleShot(0, this, [this]() { adjustSize(); });
        return;
    }

    prevCategoryId_ = id;

    int page = pageForCategory(id);
    ui_->categoryStack->setCurrentIndex(page);

    QTimer::singleShot(0, this, [this]() { adjustSize(); });
}

void ComponentEditDialog::onManufacturerChanged(int index)
{
    if (index < 0)
        return;

    const int id = ui_->manufacturerCombo->itemData(index).toInt();
    if (id != kAddNewId) {
        prevManufacturerId_ = id;
        return;
    }

    AddLookupDialog dlg(
        tr("Add Manufacturer"),
        [&](const std::string& name, DbResult& r) {
            return inventory_.manufacturers().addByName(name, r);
        },
        this
    );

    if (dlg.exec() == QDialog::Accepted) {
        populateCombos();
        QString added = QString::fromStdString(dlg.addedName());
        int newIndex = ui_->manufacturerCombo->findText(added);

        if (newIndex >= 0) {
            ui_->manufacturerCombo->setCurrentIndex(newIndex);
            prevManufacturerId_ = ui_->manufacturerCombo->itemData(newIndex).toInt();
        }
        else {
            ui_->manufacturerCombo->setCurrentIndex(0);
            prevManufacturerId_ = ui_->manufacturerCombo->itemData(0).toInt();
        }
    }
    else {
        int restoreIndex = ui_->manufacturerCombo->findData(prevManufacturerId_);
        if (restoreIndex >= 0)
            ui_->manufacturerCombo->setCurrentIndex(restoreIndex);
    }
}
