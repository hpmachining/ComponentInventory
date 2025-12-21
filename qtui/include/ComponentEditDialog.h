#pragma once

#include <QDialog>
#include "Component.h"

namespace Ui {
    class ComponentEditDialog;
}

class ComponentEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ComponentEditDialog(QWidget* parent = nullptr);
    ~ComponentEditDialog();

    void setComponent(const Component& c);
    Component component() const;

private slots:
    void onAccept();

private:
    Ui::ComponentEditDialog* ui_;
    Component component_;
};
