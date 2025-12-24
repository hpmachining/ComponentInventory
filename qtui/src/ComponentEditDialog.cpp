#include "ComponentEditDialog.h"
#include "ui_ComponentEditDialog.h"
#include "InventoryService.h"
#include "AddLookupDialog.h"

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

    populateCombos();
    prevCategoryId_ = ui_->categoryCombo->currentData().toInt();
    prevManufacturerId_ = ui_->manufacturerCombo->currentData().toInt();

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

    // Select category by ID
    int catIndex = ui_->categoryCombo->findData(c.categoryId);
    if (catIndex >= 0)
        ui_->categoryCombo->setCurrentIndex(catIndex);

    // Select manufacturer by ID
    int manIndex = ui_->manufacturerCombo->findData(c.manufacturerId);
    if (manIndex >= 0)
        ui_->manufacturerCombo->setCurrentIndex(manIndex);
}

Component ComponentEditDialog::component() const
{
    Component c = component_;

    c.partNumber = ui_->partNumberEdit->text().toStdString();
    c.description = ui_->descriptionEdit->text().toStdString();
    c.quantity = ui_->quantitySpin->value();
    c.notes = ui_->notesEdit->toPlainText().toStdString();

    c.categoryId =
        ui_->categoryCombo->currentData().toInt();

    QVariant manData = ui_->manufacturerCombo->currentData();
    c.manufacturerId =
        manData.isValid() ? manData.toInt() : 0;

    return c;
}

void ComponentEditDialog::populateCombos()
{
    DbResult result;

    // ========================
    // Categories
    // ========================
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

    // Add the "+ Add New..." option
    ui_->categoryCombo->addItem(tr("+ Add New..."), kAddNewId);

    // If previous ID not found, fallback to first real category
    if (indexToSelect < 0 && !categories.empty())
        indexToSelect = 0;

    ui_->categoryCombo->setCurrentIndex(indexToSelect);
    ui_->categoryCombo->blockSignals(false);

    // ========================
    // Manufacturers
    // ========================
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
    if (id != kAddNewId) {
        // Normal selection -> update previous id
        prevCategoryId_ = id;
        return;
    }

    // User clicked "+ Add New..."
    AddLookupDialog dlg(
        tr("Add Category"),
        [&](const std::string& name, DbResult& r) {
            return inventory_.categories().addByName(name, r);
        },
        this
    );

    if (dlg.exec() == QDialog::Accepted) {
        // Refresh combo and find the newly added category by name
        populateCombos();
        const QString added = QString::fromStdString(dlg.addedName());
        int newIndex = ui_->categoryCombo->findText(added);

        if (newIndex >= 0) {
            ui_->categoryCombo->setCurrentIndex(newIndex);
            prevCategoryId_ = ui_->categoryCombo->itemData(newIndex).toInt();
        }
        else {
            // fallback: select first valid item
            ui_->categoryCombo->setCurrentIndex(0);
            prevCategoryId_ = ui_->categoryCombo->itemData(0).toInt();
        }
    }
    else {
        // Cancelled -> restore previous selection by id
        int restoreIndex = ui_->categoryCombo->findData(prevCategoryId_);
        if (restoreIndex >= 0) {
            // Temporarily select first valid item if restoreIndex == currentIndex
            if (restoreIndex == ui_->categoryCombo->currentIndex() && restoreIndex != 0)
                ui_->categoryCombo->setCurrentIndex(0);

            ui_->categoryCombo->setCurrentIndex(restoreIndex);
        }
    }
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

//void ComponentEditDialog::onCategoryChanged(int index)
//{
//    if (index < 0)
//        return;
//
//    const int id = ui_->categoryCombo->itemData(index).toInt();
//    if (id != kAddNewId) {
//        prevCategoryIndex_ = index;
//        return;
//    }
//
//    AddLookupDialog dlg(
//        tr("Add Category"),
//        [&](const std::string& name, DbResult& r) {
//            return inventory_.categories().addByName(name, r);
//        },
//        this
//    );
//
//    if (dlg.exec() == QDialog::Accepted) {
//        populateCombos();
//
//        const QString added =
//            QString::fromStdString(dlg.addedName());
//
//        int newIndex = ui_->categoryCombo->findText(added);
//        if (newIndex >= 0)
//            ui_->categoryCombo->setCurrentIndex(newIndex);
//    }
//    else {
//        ui_->categoryCombo->setCurrentIndex(prevCategoryIndex_);
//    }
//}
//
//void ComponentEditDialog::onManufacturerChanged(int index)
//{
//    if (index < 0)
//        return;
//
//    const int id = ui_->manufacturerCombo->itemData(index).toInt();
//    if (id != kAddNewId) {
//        prevManufacturerIndex_ = index;
//        return;
//    }
//
//    AddLookupDialog dlg(
//        tr("Add Manufacturer"),
//        [&](const std::string& name, DbResult& r) {
//            return inventory_.manufacturers().addByName(name, r);
//        },
//        this
//    );
//
//    if (dlg.exec() == QDialog::Accepted) {
//        populateCombos();
//
//        const QString added =
//            QString::fromStdString(dlg.addedName());
//
//        int newIndex = ui_->manufacturerCombo->findText(added);
//        if (newIndex >= 0)
//            ui_->manufacturerCombo->setCurrentIndex(newIndex);
//    }
//    else {
//        ui_->manufacturerCombo->setCurrentIndex(prevManufacturerIndex_);
//    }
//}
