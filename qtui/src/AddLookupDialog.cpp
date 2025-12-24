#include "AddLookupDialog.h"
#include "DbResult.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QMessageBox>

AddLookupDialog::AddLookupDialog(
    const QString& title,
    AddFn addFn,
    QWidget* parent)
    : QDialog(parent),
    addFn_(std::move(addFn))
{
    setWindowTitle(title);
    setModal(true);

    auto* nameLabel = new QLabel(tr("Name:"));
    nameEdit_ = new QLineEdit(this);
    nameEdit_->setPlaceholderText(tr("Enter name"));

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        this
    );

    connect(buttons, &QDialogButtonBox::accepted,
        this, &AddLookupDialog::onAddClicked);
    connect(buttons, &QDialogButtonBox::rejected,
        this, &QDialog::reject);

    auto* layout = new QGridLayout(this);
    layout->addWidget(nameLabel, 0, 0);
    layout->addWidget(nameEdit_, 0, 1);
    layout->addWidget(buttons, 1, 0, 1, 2);

    nameEdit_->setFocus();
}

void AddLookupDialog::onAddClicked()
{
    DbResult res;
    const std::string name = nameEdit_->text().toStdString();

    if (!addFn_(name, res)) {
        QMessageBox::warning(
            this,
            tr("Error"),
            QString::fromStdString(res.toString())
        );
        return;
    }

    addedName_ = name;
    accept();
}
