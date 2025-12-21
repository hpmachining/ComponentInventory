#include "ComponentEditDialog.h"
#include "ui_ComponentEditDialog.h"
#include <QMessageBox>

ComponentEditDialog::ComponentEditDialog(QWidget* parent)
    : QDialog(parent),
    ui_(new Ui::ComponentEditDialog)
{
    ui_->setupUi(this);

    // Optional but recommended
    setWindowTitle(tr("Edit Component"));

    // OK / Cancel are auto-wired if you used QDialogButtonBox
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

    ui_->partNumberEdit->setText(
        QString::fromStdString(c.partNumber)
    );

    ui_->descriptionEdit->setText(
        QString::fromStdString(c.description)
    );

    ui_->quantitySpin->setValue(c.quantity);

    ui_->notesEdit->setPlainText(
        QString::fromStdString(c.notes)
    );
}

Component ComponentEditDialog::component() const
{
    Component c = component_;

    c.partNumber = ui_->partNumberEdit->text().toStdString();
    c.description = ui_->descriptionEdit->text().toStdString();
    c.quantity = ui_->quantitySpin->value();
    c.notes = ui_->notesEdit->toPlainText().toStdString();

    return c;
}
