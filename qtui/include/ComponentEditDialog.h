#pragma once

#include <QDialog>
#include "ComponentManager.h"

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
    // New: override accept to save subtype data
    void accept() override;

    bool isValid() const;
    void onCategoryChanged(int index);
    void onManufacturerChanged(int index);
    void populateCombos();

    // New: helpers
    int pageForCategory(int categoryId) const;
    void populateResistorLookups();
    void loadResistorFields(int componentId);
    void saveResistorFields(int componentId);

    // New: page indices for the stacked widget
    enum CategoryPage {
        Page_None = 0,
        Page_Resistor = 1,
        // Future: Page_Capacitor = 2, etc.
    };

    Ui::ComponentEditDialog* ui_;
    InventoryService& inventory_;
    Component component_;

    static constexpr int kAddNewId = -1;
    int prevCategoryId_ = -1;
    int prevManufacturerId_ = -1;
};
