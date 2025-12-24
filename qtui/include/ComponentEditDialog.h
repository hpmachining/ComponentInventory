#pragma once

#include <QDialog>
#include "Component.h"

namespace Ui {
    class ComponentEditDialog;
}

class InventoryService;

class ComponentEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ComponentEditDialog(
        InventoryService& inventory,
        QWidget* parent = nullptr
    );
    ~ComponentEditDialog();

    void setComponent(const Component& c);
    Component component() const;

private slots:
    void updateOkButtonState();

private:
    bool isValid() const;
    void onCategoryChanged(int index);
    void onManufacturerChanged(int index);
    void populateCombos();

    Ui::ComponentEditDialog* ui_;
    InventoryService& inventory_;
    Component component_;
    
    static constexpr int kAddNewId = -1;
    int prevCategoryId_ = -1;
    int prevManufacturerId_ = -1;
};
