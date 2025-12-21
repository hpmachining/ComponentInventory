#include "ComponentEditDialog.h"
#include "ui_ComponentEditDialog.h"

ComponentEditDialog::ComponentEditDialog(QWidget* parent)
    : QDialog(parent),
    ui_(new Ui::ComponentEditDialog)
{
    ui_->setupUi(this);

    // Optional but recommended
    setWindowTitle(tr("Edit Component"));

    // OK / Cancel are auto-wired if you used QDialogButtonBox
}

ComponentEditDialog::~ComponentEditDialog()
{
    delete ui_;
}

void ComponentEditDialog::setComponent(const Component& c)
{
    component_ = c;

    // Phase 2B:
    // Populate UI fields from component_
}

Component ComponentEditDialog::component() const
{
    // Phase 2B:
    // Read values from UI fields into component_

    return component_;
}
