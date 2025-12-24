#include "ComponentEditDialog.h"
#include "ui_ComponentEditDialog.h"
#include "InventoryService.h"
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

    // Disable OK initially
    ui_->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    // Live validation
    connect(ui_->partNumberEdit, &QLineEdit::textChanged,
        this, &ComponentEditDialog::updateOkButtonState);
    connect(ui_->categoryCombo, &QComboBox::currentIndexChanged,
        this, &ComponentEditDialog::updateOkButtonState);
    connect(ui_->manufacturerCombo, &QComboBox::currentIndexChanged,
        this, &ComponentEditDialog::updateOkButtonState);
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

    // ---- Categories ----
    ui_->categoryCombo->clear();
    std::vector<LookupItem> categories;

    if (inventory_.categories().listLookup(categories, result)) {
        for (const auto& c : categories) {
            ui_->categoryCombo->addItem(
                QString::fromStdString(c.name),
                c.id
            );
        }
    }

    // ---- Manufacturers ----
    ui_->manufacturerCombo->clear();
    std::vector<LookupItem> manufacturers;

    if (inventory_.manufacturers().listLookup(manufacturers, result)) {
        for (const auto& m : manufacturers) {
            ui_->manufacturerCombo->addItem(
                QString::fromStdString(m.name),
                m.id
            );
        }
    }

    int genericIndex =
        ui_->manufacturerCombo->findText("Generic");
    if (genericIndex >= 0)
        ui_->manufacturerCombo->setCurrentIndex(genericIndex);
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
