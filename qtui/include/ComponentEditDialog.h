#pragma once

#include "ComponentManager.h"
#include "ResistorManager.h"
#include "editors/IComponentEditor.h"
#include <QDialog>
#include <memory>

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
    ~ComponentEditDialog() override;

    void setComponent(const Component& c);
    Component component() const;

private slots:
    void updateOkButtonState();
    void onCategoryChanged(int index);
    void onManufacturerChanged(int index);

private:
    void accept() override;
    bool isValid() const;
    void setTypeEditor(std::unique_ptr<IComponentEditor> editor);
    void populateLookups();
    int pageForCategory(int categoryId) const;

    // Page indices for stacked widget
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

    std::unique_ptr<IComponentEditor> typeEditor_;
    QWidget* typeEditorContainer_{ nullptr };
};
