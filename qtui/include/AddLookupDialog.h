#pragma once

#include <QDialog>
#include <QLineEdit>
#include <functional>
#include <string>

class DbResult;

class AddLookupDialog : public QDialog {
    Q_OBJECT
public:
    using AddFn = std::function<bool(const std::string&, DbResult&)>;

    AddLookupDialog(
        const QString& title,
        AddFn addFn,
        QWidget* parent = nullptr
    );

    std::string addedName() const { return addedName_; }

private slots:
    void onAddClicked();

private:
    AddFn addFn_;
    std::string addedName_;

    QLineEdit* nameEdit_;
};
