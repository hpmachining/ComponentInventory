
#include "ComponentEditDialog.h"
#include "ui_ComponentEditDialog.h"
#include "InventoryService.h"
#include "AddLookupDialog.h"
#include "DbResult.h"
#include "editors/ResistorEditor.h"
#include "editors/CapacitorEditor.h"

#include <QMessageBox>
#include <QDialogButtonBox>
#include <QTimer>
#include <QVBoxLayout>
#include <QPushButton>

static QWidget* firstFocusableChild(QWidget* parent)
{
    QList<QWidget*> widgets = parent->findChildren<QWidget*>();
    for (QWidget* w : widgets) {
        if (w->focusPolicy() != Qt::NoFocus)
            return w;
    }
    return nullptr;
}

static QWidget* lastFocusableChild(QWidget* parent)
{
    QList<QWidget*> widgets = parent->findChildren<QWidget*>();
    QWidget* last = nullptr;
    for (QWidget* w : widgets) {
        if (w->focusPolicy() != Qt::NoFocus)
            last = w;
    }
    return last;
}

ComponentEditDialog::ComponentEditDialog(
    InventoryService& inventory,
    QWidget* parent
)
    : BaseDialog(parent),
    ui_(new Ui::ComponentEditDialog),
    inventory_(inventory)
{
    ui_->setupUi(this);
    setWindowTitle(tr("Add Component"));


    // Type editor container
    typeEditorContainer_ = new QWidget(this);
    auto* typeLayout = new QVBoxLayout(typeEditorContainer_);
    typeLayout->setContentsMargins(0, 0, 0, 0);
    typeLayout->setSpacing(0);

    // Insert just above the button box
    ui_->verticalLayout->insertWidget(
        ui_->verticalLayout->count() - 1,
        typeEditorContainer_
    );

    // Tab moves focus in notesEdit
    ui_->notesEdit->setTabChangesFocus(true);

    populateLookups();
    ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    prevCategoryId_ = ui_->categoryCombo->currentData().toInt();
    prevManufacturerId_ = ui_->manufacturerCombo->currentData().toInt();

    // After populateLookups() and setting prevCategoryId_
    int initialCategoryId = ui_->categoryCombo->currentData().toInt();
    int initialPage = pageForCategory(initialCategoryId);

    if (initialPage == Page_Resistor) {
        setTypeEditor(std::make_unique<ResistorEditor>(inventory_));
    }
    else if (initialPage == Page_Capacitor) {
        setTypeEditor(std::make_unique<CapacitorEditor>(inventory_));
    }
    else {
        setTypeEditor(nullptr);
    }

    // Load subtype data if editing an existing component
    if (typeEditor_ && component_.id > 0)
        typeEditor_->load(component_.id);
    
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
    ui_->datasheetEdit->setText(QString::fromStdString(c.datasheetLink));

    // Select category
    int catIndex = ui_->categoryCombo->findData(c.categoryId);
    if (catIndex >= 0)
        ui_->categoryCombo->setCurrentIndex(catIndex);

    // Select manufacturer
    int manIndex = ui_->manufacturerCombo->findData(c.manufacturerId);
    if (manIndex >= 0)
        ui_->manufacturerCombo->setCurrentIndex(manIndex);

    // Switch category page
    int page = pageForCategory(c.categoryId);

    // Let type editor load subtype fields
    if (typeEditor_)
        typeEditor_->load(c.id); // use component ID

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

void ComponentEditDialog::populateLookups()
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

    if (name.compare("Capacitor", Qt::CaseInsensitive) == 0)   // NEW
        return Page_Capacitor;

    return Page_None;
}

void ComponentEditDialog::accept()
{
    if (!isValid()) {
        QMessageBox::warning(this, tr("Invalid Input"),
            tr("Please enter a part number."));
        return;
    }

    // Update in-memory component
    component_ = component();

    // Extract subtype data (NO DB WRITES)
    DbResult result;
    if (typeEditor_) {
        if (!typeEditor_->collect(component_.id, result)) {
            QMessageBox::warning(this, "Error",
                QString::fromStdString(result.message));
            return;
        }
    }

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
        // ... existing Add Category logic unchanged ...
        // (keep all of this block exactly as-is)
    }
    else {
        prevCategoryId_ = id;
    }

    // Instantiate / assign the correct type editor for this category
    int page = pageForCategory(id);
    if (page == Page_Resistor) {
        setTypeEditor(std::make_unique<ResistorEditor>(inventory_));
    }
    else if (page == Page_Capacitor) {                    // NEW
        setTypeEditor(std::make_unique<CapacitorEditor>(inventory_));
    }
    else {
        setTypeEditor(nullptr);
    }

    // Let the editor load subtype fields (if a component is already set)
    if (typeEditor_ && component_.id > 0)                 // slightly safer
        typeEditor_->load(component_.id);

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
        populateLookups();
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

void ComponentEditDialog::setTypeEditor(std::unique_ptr<IComponentEditor> editor)
{
    typeEditor_.reset();
    QLayout* layout = typeEditorContainer_->layout();

    // Clear existing widget
    while (QLayoutItem* item = layout->takeAt(0)) {
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }

    typeEditor_ = std::move(editor);

    if (typeEditor_) {
        QWidget* w = typeEditor_->widget();
        layout->addWidget(w);

        QWidget* first = firstFocusableChild(w);
        QWidget* last = lastFocusableChild(w);

        if (first && last) {
            // 1. Dialog fields → first child field
            setTabOrder(ui_->notesEdit, first);

            // 2. Last child field → OK button
            QPushButton* okBtn = ui_->buttonBox->button(QDialogButtonBox::Ok);
            setTabOrder(last, okBtn);

            // 3. OK → Cancel (Qt normally does this automatically)
            QPushButton* cancelBtn = ui_->buttonBox->button(QDialogButtonBox::Cancel);
            setTabOrder(okBtn, cancelBtn);
        }
    }
}
