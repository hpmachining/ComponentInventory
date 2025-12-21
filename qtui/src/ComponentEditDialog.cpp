#include "ComponentEditDialog.h"
#include "ui_ComponentEditDialog.h"
#include "InventoryService.h"
#include <QMessageBox>

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

    populateCombos();

    connect(ui_->buttonBox, &QDialogButtonBox::accepted,
        this, &ComponentEditDialog::onAccept);
}

ComponentEditDialog::~ComponentEditDialog()
{
    delete ui_;
}

void ComponentEditDialog::onAccept()
{
    if (ui_->partNumberEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(
            this,
            tr("Missing Field"),
            tr("Part Number is required.")
        );
        return;
    }

    accept();
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

    // ---- Categories ----
    ui_->categoryCombo->clear();

    std::vector<Category> categories;
    if (inventory_.categories().listCategories(categories, result)) {
        for (const auto& c : categories) {
            ui_->categoryCombo->addItem(
                QString::fromStdString(c.name),
                c.id   // stored as user data
            );
        }
    }

    // ---- Manufacturers ----
    ui_->manufacturerCombo->clear();

    std::vector<Manufacturer> manufacturers;
    if (inventory_.manufacturers().listManufacturers(manufacturers, result)) {
        // Optional: allow "no manufacturer"
        ui_->manufacturerCombo->addItem(tr("<None>"), QVariant());

        for (const auto& m : manufacturers) {
            ui_->manufacturerCombo->addItem(
                QString::fromStdString(m.name),
                m.id
            );
        }
    }
}
