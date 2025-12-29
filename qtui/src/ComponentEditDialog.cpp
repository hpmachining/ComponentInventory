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

    // New: start on "no category-specific fields"
    ui_->categoryStack->setCurrentIndex(Page_None);

    populateCombos();
    prevCategoryId_ = ui_->categoryCombo->currentData().toInt();
    prevManufacturerId_ = ui_->manufacturerCombo->currentData().toInt();

    // New: populate resistor lookup combos
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
    //ui_->datasheetEdit->setText(QString::fromStdString(c.datasheetLink)); // assuming field

    // Select category by ID
    int catIndex = ui_->categoryCombo->findData(c.categoryId);
    if (catIndex >= 0)
        ui_->categoryCombo->setCurrentIndex(catIndex);

    // Select manufacturer by ID
    int manIndex = ui_->manufacturerCombo->findData(c.manufacturerId);
    if (manIndex >= 0)
        ui_->manufacturerCombo->setCurrentIndex(manIndex);

    // New: set the stacked page based on category
    int page = pageForCategory(c.categoryId);
    ui_->categoryStack->setCurrentIndex(page);

    // New: load category-specific fields
    if (page == Page_Resistor && c.id > 0) {
        loadResistorFields(c.id);
    }
}

Component ComponentEditDialog::component() const
{
    Component c = component_;

    c.partNumber = ui_->partNumberEdit->text().toStdString();
    c.description = ui_->descriptionEdit->text().toStdString();
    c.quantity = ui_->quantitySpin->value();
    c.notes = ui_->notesEdit->toPlainText().toStdString();
    c.datasheetLink = ui_->datasheetEdit->text().toStdString(); // new

    c.categoryId = ui_->categoryCombo->currentData().toInt();

    QVariant manData = ui_->manufacturerCombo->currentData();
    c.manufacturerId = manData.isValid() ? manData.toInt() : 0;

    return c;
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
            ui_->categoryCombo->addItem(
                QString::fromStdString(c.name),
                c.id
            );

            if (c.id == prevCategoryId_)
                indexToSelect = index;
        }
    }

    // Fallback to first category if previous not found
    if (indexToSelect < 0 && !categories.empty())
        indexToSelect = 0;

    ui_->categoryCombo->setCurrentIndex(indexToSelect);
    ui_->categoryCombo->blockSignals(false);

    // Manufacturers (Add New allowed)
    ui_->manufacturerCombo->blockSignals(true);
    ui_->manufacturerCombo->clear();

    indexToSelect = -1;

    std::vector<LookupItem> manufacturers;
    if (inventory_.manufacturers().listLookup(manufacturers, result)) {
        for (const auto& m : manufacturers) {
            int index = ui_->manufacturerCombo->count();
            ui_->manufacturerCombo->addItem(
                QString::fromStdString(m.name),
                m.id
            );

            if (m.id == prevManufacturerId_)
                indexToSelect = index;
        }
    }

    // Add the "+ Add New..." option
    ui_->manufacturerCombo->addItem(tr("+ Add New..."), kAddNewId);

    // Fallback to first real manufacturer if previous not found
    if (indexToSelect < 0 && !manufacturers.empty())
        indexToSelect = 0;

    ui_->manufacturerCombo->setCurrentIndex(indexToSelect);
    ui_->manufacturerCombo->blockSignals(false);
}

int ComponentEditDialog::pageForCategory(int categoryId) const
{
    QString name;

    int idx = ui_->categoryCombo->findData(categoryId);
    if (idx >= 0)
        name = ui_->categoryCombo->itemText(idx);

    if (name.compare("Resistor", Qt::CaseInsensitive) == 0)
        return Page_Resistor;

    return Page_None;
}

void ComponentEditDialog::populateResistorLookups()
{
    DbResult result;

    // --- Packages ---
    ui_->resistorPackageCombo->clear();
    std::vector<ResistorPackage> pkgs;
    if (inventory_.resistorPackages().list(pkgs, result)) {
        for (const auto& p : pkgs) {
            ui_->resistorPackageCombo->addItem(
                QString::fromStdString(p.name),
                p.id
            );
        }
    }

    // --- Compositions ---
    ui_->resistorCompositionCombo->clear();
    std::vector<ResistorComposition> comps;
    if (inventory_.resistorCompositions().list(comps, result)) {
        for (const auto& c : comps) {
            ui_->resistorCompositionCombo->addItem(
                QString::fromStdString(c.name),
                c.id
            );
        }
    }
}

void ComponentEditDialog::loadResistorFields(int componentId)
{
    DbResult result;
    Resistor r;

    if (!inventory_.resistors().getByComponentId(componentId, r, result)) {
        return; // No resistor row yet
    }

    ui_->resistanceSpin->setValue(r.resistance);
    ui_->toleranceSpin->setValue(r.tolerance);
    ui_->powerSpin->setValue(r.powerRating);
    ui_->tempCoeffSpin->setValue(r.tempCoefficient);
    ui_->voltageSpin->setValue(r.voltageRating);
    ui_->leadSpacingSpin->setValue(r.leadSpacing);

    populateResistorLookups();

    int pkgIndex = ui_->resistorPackageCombo->findData(r.packageTypeId);
    if (pkgIndex >= 0)
        ui_->resistorPackageCombo->setCurrentIndex(pkgIndex);

    int compIndex = ui_->resistorCompositionCombo->findData(r.compositionId);
    if (compIndex >= 0)
        ui_->resistorCompositionCombo->setCurrentIndex(compIndex);
}

void ComponentEditDialog::saveResistorFields(int componentId)
{
    Resistor r;
    r.componentId = componentId;
    r.resistance = ui_->resistanceSpin->value();
    r.tolerance = ui_->toleranceSpin->value();
    r.powerRating = ui_->powerSpin->value();
    r.tempCoefficient = 0.0; // You can add a UI field later
    r.voltageRating = ui_->voltageSpin->value();
    r.leadSpacing = ui_->leadSpacingSpin->value();
    r.packageTypeId = ui_->resistorPackageCombo->currentData().toInt();
    r.compositionId = ui_->resistorCompositionCombo->currentData().toInt();

    DbResult result;
    Resistor existing;

    // UPDATE if exists
    if (inventory_.resistors().getByComponentId(componentId, existing, result)) {
        if (!inventory_.resistors().update(r, result)) {
            QMessageBox::warning(
                this,
                tr("Error"),
                tr("Failed to update resistor details: %1")
                .arg(QString::fromStdString(result.message))
            );
        }
        return;
    }

    // INSERT if not exists
    result.clear();
    if (!inventory_.resistors().add(r, result)) {
        QMessageBox::warning(
            this,
            tr("Error"),
            tr("Failed to insert resistor details: %1")
            .arg(QString::fromStdString(result.message))
        );
    }
}

void ComponentEditDialog::accept()
{
    if (!isValid()) {
        QMessageBox::warning(
            this,
            tr("Invalid Input"),
            tr("Please enter a part number.")
        );
        return;
    }

    DbResult result;
    Component c = component();

    int componentId = c.id;

    // INSERT
    if (componentId == 0) {
        if (!inventory_.components().addComponent(c, result)) {
            QMessageBox::warning(
                this,
                tr("Error"),
                tr("Failed to insert component: %1")
                .arg(QString::fromStdString(result.message))
            );
            return;
        }

        // After addComponent(), the ComponentManager should set c.id internally.
        // If it does not, you must modify addComponent() to return the new ID.
        componentId = c.id;
        component_.id = c.id;
    }
    else {
        // UPDATE
        if (!inventory_.components().updateComponent(c, result)) {
            QMessageBox::warning(
                this,
                tr("Error"),
                tr("Failed to update component: %1")
                .arg(QString::fromStdString(result.message))
            );
            return;
        }
    }

    // Save subtype fields
    int page = pageForCategory(c.categoryId);
    if (page == Page_Resistor)
        saveResistorFields(componentId);

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
        // existing "+ Add New..." logic
        AddLookupDialog dlg(
            tr("Add Category"),
            [&](const std::string& name, DbResult& r) {
                return inventory_.categories().addByName(name, r);
            },
            this
        );

        if (dlg.exec() == QDialog::Accepted) {
            populateCombos();
            const QString added = QString::fromStdString(dlg.addedName());
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
            if (restoreIndex >= 0) {
                if (restoreIndex == ui_->categoryCombo->currentIndex() && restoreIndex != 0)
                    ui_->categoryCombo->setCurrentIndex(0);
                ui_->categoryCombo->setCurrentIndex(restoreIndex);
            }
        }

        // After add/cancel, we still want the correct page set
        int currentId = ui_->categoryCombo->currentData().toInt();
        int page = pageForCategory(currentId);
        ui_->categoryStack->setCurrentIndex(page);
        return;
    }

    // Normal selection
    prevCategoryId_ = id;

    // New: switch stacked widget page
    int page = pageForCategory(id);
    ui_->categoryStack->setCurrentIndex(page);
}

void ComponentEditDialog::onManufacturerChanged(int index)
{
    if (index < 0)
        return;

    const int id = ui_->manufacturerCombo->itemData(index).toInt();
    if (id != kAddNewId) {
        // Normal selection -> update previous id
        prevManufacturerId_ = id;
        return;
    }

    // User clicked "+ Add New..."
    AddLookupDialog dlg(
        tr("Add Manufacturer"),
        [&](const std::string& name, DbResult& r) {
            return inventory_.manufacturers().addByName(name, r);
        },
        this
    );

    if (dlg.exec() == QDialog::Accepted) {
        // Refresh combo and find the newly added manufacturer by name
        populateCombos();
        const QString added = QString::fromStdString(dlg.addedName());
        int newIndex = ui_->manufacturerCombo->findText(added);

        if (newIndex >= 0) {
            ui_->manufacturerCombo->setCurrentIndex(newIndex);
            prevManufacturerId_ = ui_->manufacturerCombo->itemData(newIndex).toInt();
        }
        else {
            // fallback: select first valid item
            ui_->manufacturerCombo->setCurrentIndex(0);
            prevManufacturerId_ = ui_->manufacturerCombo->itemData(0).toInt();
        }
    }
    else {
        // Cancelled -> restore previous selection by id
        int restoreIndex = ui_->manufacturerCombo->findData(prevManufacturerId_);
        if (restoreIndex >= 0)
            ui_->manufacturerCombo->setCurrentIndex(restoreIndex);
    }
}
