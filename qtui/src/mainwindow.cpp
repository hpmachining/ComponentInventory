#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_ComponentEditDialog.h"
#include "SchemaManager.h"
#include "InventoryService.h"
#include "ComponentTableModel.h"
#include "ComponentEditDialog.h"
#include "editors/ResistorEditor.h"
#include "DevDataSeeder.h"
#include <QMessageBox>
#include <QStatusBar>
#include <QFileDialog>
#include <QVBoxLayout>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Allow clearing selection by clicking empty table space
    ui->componentView->viewport()->installEventFilter(this);

    // --- Header appearance ---
    // Force bold header font regardless of focus/selection state
    QFont headerFont = ui->componentView->horizontalHeader()->font();
    headerFont.setBold(true);
    ui->componentView->horizontalHeader()->setFont(headerFont);

    // --- Central widget & layout ---
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* layout = new QVBoxLayout(central);
    layout->addWidget(ui->componentView);
    layout->setContentsMargins(0, 0, 0, 0);

    // --- Table model ---
    componentModel_ = new ComponentTableModel(this); // DB set later via InventoryService
    ui->componentView->setModel(componentModel_);

    // --- Header resize behavior ---
    ui->componentView->horizontalHeader()
        ->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    for (int i = 1; i < ui->componentView->model()->columnCount(); ++i)
        ui->componentView->horizontalHeader()
        ->setSectionResizeMode(i, QHeaderView::Stretch);

    // --- Selection behavior ---
    ui->componentView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->componentView->setSelectionMode(QAbstractItemView::SingleSelection);

    // --- Table styling ---
    ui->componentView->setStyleSheet(
        "QTableView::item:hover {"
        "  background: rgba(30, 144, 255, 40);"
        "}"
        "QTableView::item:selected {"
        "  background: palette(highlight);"
        "  color: palette(highlighted-text);"
        "}"
        "QTableView::item:selected:!active {"
        "  background: palette(highlight);"
        "  color: palette(highlighted-text);"
        "}"
        "QTableView::item {"
        "  border: none;"
        "}"
    );

    // --- Window & status ---
    updateWindowTitle();
    statusBar()->showMessage(tr("Ready"));
    disableDatabaseActions();

	connectSelectionModel();

    // --- Keyboard shortcuts ---
    // Enter / Return edits selected component
    ui->actionEditComponent->setShortcut(Qt::Key_Return);

    // Delete key deletes selected component
    ui->actionDeleteComponent->setShortcut(Qt::Key_Delete);

    // --- Action connections ---
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onActionExit);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onActionAbout);
    connect(ui->actionNewDatabase, &QAction::triggered, this, &MainWindow::onActionNewDatabase);
    connect(ui->actionOpenDatabase, &QAction::triggered, this, &MainWindow::onActionOpenDatabase);
    connect(ui->actionCloseDatabase, &QAction::triggered, this, &MainWindow::onActionCloseDatabase);
    connect(ui->actionAddTestComponent, &QAction::triggered, this, &MainWindow::onActionAddTestComponent);
    connect(ui->actionAddComponent, &QAction::triggered, this, &MainWindow::onActionAddComponent);
    connect(ui->actionEditComponent, &QAction::triggered, this, &MainWindow::onActionEditComponent);
    connect(ui->actionDeleteComponent, &QAction::triggered, this, &MainWindow::onActionDeleteComponent);

    // Double-click row → edit
    connect(ui->componentView, &QTableView::doubleClicked, this, &MainWindow::onActionEditComponent);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    closeDatabase();          // cleanup logic
    event->accept();    // allow the window to close
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == ui->componentView->viewport() &&
        event->type() == QEvent::MouseButtonPress)
    {
        auto* mouseEvent = static_cast<QMouseEvent*>(event);

        // Which index is under the mouse?
        QModelIndex index =
            ui->componentView->indexAt(mouseEvent->pos());

        if (!index.isValid())
        {
            // Clicked empty space → deselect
            ui->componentView->selectionModel()->clearSelection();
            ui->componentView->clearFocus(); // optional, feels good
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

// --- Slots ---

void MainWindow::onActionExit()
{
    close();
}

void MainWindow::onActionAbout()
{
    QMessageBox::about(this,
        tr("About ComponentInventory"),
        tr("ComponentInventory\n\nQt6 front end for your SQLite database."));
}

void MainWindow::onActionNewDatabase()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Create New Database"),
        QDir::homePath() + "/inventory.db",
        tr("SQLite Database (*.db *.sqlite)")
    );

    if (fileName.isEmpty())
        return;

    createNewDatabase(fileName);
}

void MainWindow::onActionOpenDatabase()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open Database"),
        QDir::homePath(),
        tr("SQLite Database (*.db *.sqlite)")
    );

    if (fileName.isEmpty())
        return;

    openExistingDatabase(fileName);
}

void MainWindow::onActionCloseDatabase()
{
    closeDatabase();
}

void MainWindow::onActionAddComponent()
{
    if (!inventory_)
        return;

    ComponentEditDialog dialog(*inventory_, this);

    Component c; // blank component
    dialog.setComponent(c);

    if (dialog.exec() != QDialog::Accepted)
        return;

    // 1. Extract base component fields from dialog
    c = dialog.component();
    DbResult result;

    // 2. Insert base component first (assigns c.id)
    if (!inventory_->components().add(c, result)) {
        QMessageBox::critical(this, tr("Error"),
            QString::fromStdString(result.toString()));
        return;
    }

    // 3. Retrieve subtype editor (if any)
    IComponentEditor* editor = dialog.typeEditor();
    if (editor) {

        // Extract subtype model from editor (NO DB writes)
        DbResult subResult;
        if (!editor->collect(c.id, subResult)) {
            QMessageBox::critical(this, tr("Subtype Error"),
                QString::fromStdString(subResult.toString()));
            return;
        }

        // 4. Save subtype based on category
        switch (c.categoryId) {

        case 1: { // Resistor
            auto* rEditor = dynamic_cast<ResistorEditor*>(editor);
            if (rEditor) {
                const Resistor& r = rEditor->resistor();

                // Check if resistor row already exists
                Resistor existing;
                DbResult check;
                bool exists =
                    inventory_->resistors().getByComponentId(c.id, existing, check);

                if (exists) {
                    // UPDATE
                    if (!inventory_->resistors().update(r, result)) {
                        QMessageBox::critical(this, tr("Error"),
                            QString::fromStdString(result.toString()));
                        return;
                    }
                }
                else {
                    // INSERT
                    if (!inventory_->resistors().add(r, result)) {
                        QMessageBox::critical(this, tr("Error"),
                            QString::fromStdString(result.toString()));
                        return;
                    }
                }
            }
            break;
        }

              // Future:
              // case 2: Capacitor
              // case 3: Transistor
              // case 4: Diode
              // case 5: Fuse

        default:
            break;
        }
    }

    // 5. Refresh UI
    reloadComponents();
    statusBar()->showMessage(tr("Component added"), 3000);
}

void MainWindow::onActionDeleteComponent()
{
    if (!inventory_ || !componentModel_)
        return;

    auto selection = ui->componentView->selectionModel();
    if (!selection || !selection->hasSelection()) {
        QMessageBox::information(
            this,
            tr("Delete Component"),
            tr("Please select a component to delete.")
        );
        return;
    }

    int row = selection->selectedRows().first().row();
    int componentId = componentModel_->componentIdAt(row);

    // Pull display values from the model
    QString partNumber =
        componentModel_->data(componentModel_->index(row, 2), Qt::DisplayRole).toString();
    QString category =
        componentModel_->data(componentModel_->index(row, 1), Qt::DisplayRole).toString();
    QString manufacturer =
        componentModel_->data(componentModel_->index(row, 3), Qt::DisplayRole).toString();

    QString message = tr(
        "Are you sure you want to delete this component?\n\n"
        "Part Number: %1\n"
        "Category: %2\n"
        "Manufacturer: %3"
    ).arg(partNumber, category, manufacturer);

    auto reply = QMessageBox::warning(
        this,
        tr("Confirm Delete"),
        message,
        QMessageBox::Yes | QMessageBox::Cancel,
        QMessageBox::Cancel
    );

    if (reply != QMessageBox::Yes)
        return;

    DbResult result;
    if (!inventory_->components().remove(componentId, result)) {
        QMessageBox::critical(
            this,
            tr("Error"),
            QString::fromStdString(result.toString())
        );
        return;
    }

    reloadComponents();
    statusBar()->showMessage(tr("Component deleted"), 3000);
}

void MainWindow::onActionAddTestComponent()
{
    if (!inventory_)
        return;

    Component c;
    c.partNumber = "TEST-" + std::to_string(QDateTime::currentSecsSinceEpoch());
    c.categoryId = 1;        // Resistor
    c.manufacturerId = 1;    // Generic
    c.description = "Phase 1 test component";
    c.quantity = 1;

    DbResult result;
    if (!inventory_->components().add(c, result)) {
        QMessageBox::critical(this, tr("Error"), QString::fromStdString(result.toString()));
        return;
    }

    reloadComponents();
}

void MainWindow::onActionEditComponent()
{
    if (!inventory_ || !componentModel_)
        return;

    auto index = ui->componentView->currentIndex();
    if (!index.isValid())
        return;

    int row = index.row();
    int componentId = componentModel_->componentIdAt(row);

    DbResult result;
    Component c;

    // Load base component
    if (!inventory_->components().getById(componentId, c, result)) {
        QMessageBox::critical(this, tr("Error"),
            QString::fromStdString(result.toString()));
        return;
    }

    // Open dialog
    ComponentEditDialog dialog(*inventory_, this);
    dialog.setComponent(c);

    if (dialog.exec() != QDialog::Accepted)
        return;

    // 1. Extract updated base component fields
    c = dialog.component();

    // 2. Update base component row
    if (!inventory_->components().update(c, result)) {
        QMessageBox::critical(this, tr("Error"),
            QString::fromStdString(result.toString()));
        return;
    }

    // 3. Extract subtype editor (if any)
    IComponentEditor* editor = dialog.typeEditor();
    if (editor) {

        // Extract subtype model (NO DB writes)
        DbResult subResult;
        if (!editor->collect(c.id, subResult)) {
            QMessageBox::critical(this, tr("Subtype Error"),
                QString::fromStdString(subResult.toString()));
            return;
        }

        // 4. Save subtype based on category
        switch (c.categoryId) {

        case 1: { // Resistor
            auto* rEditor = dynamic_cast<ResistorEditor*>(editor);
            if (rEditor) {
                const Resistor& r = rEditor->resistor();

                // Check if resistor row exists
                Resistor existing;
                DbResult check;
                bool exists =
                    inventory_->resistors().getByComponentId(c.id, existing, check);

                if (exists) {
                    // UPDATE resistor row
                    if (!inventory_->resistors().update(r, result)) {
                        QMessageBox::critical(this, tr("Error"),
                            QString::fromStdString(result.toString()));
                        return;
                    }
                }
                else {
                    // INSERT resistor row
                    if (!inventory_->resistors().add(r, result)) {
                        QMessageBox::critical(this, tr("Error"),
                            QString::fromStdString(result.toString()));
                        return;
                    }
                }
            }
            break;
        }

              // Future:
              // case 2: Capacitor
              // case 3: Transistor
              // case 4: Diode
              // case 5: Fuse

        default:
            break;
        }
    }

    // 5. Refresh UI
    reloadComponents();
    statusBar()->showMessage(tr("Component updated"), 3000);
}

// --- Database lifecycle helpers ---

void MainWindow::clearComponentView()
{
    if (componentModel_)
        componentModel_->setComponents({});
}

void MainWindow::reloadComponents()
{
    if (!inventory_ || !componentModel_)
        return;

    DbResult result;

    // Load components
    std::vector<Component> components;
    if (!inventory_->components().list(components, result)) {
        QMessageBox::critical(this, tr("Error"),
            QString::fromStdString(result.toString()));
        return;
    }

    // Load categories
    std::vector<Category> categories;
    inventory_->categories().list(categories, result);

    std::unordered_map<int, QString> categoryMap;
    for (const auto& c : categories)
        categoryMap[c.id] = QString::fromStdString(c.name);

    // Load manufacturers
    std::vector<Manufacturer> manufacturers;
    inventory_->manufacturers().list(manufacturers, result);

    std::unordered_map<int, QString> manufacturerMap;
    for (const auto& m : manufacturers)
        manufacturerMap[m.id] = QString::fromStdString(m.name);

    componentModel_->setCategoryLookup(std::move(categoryMap));
    componentModel_->setManufacturerLookup(std::move(manufacturerMap));
    componentModel_->setComponents(std::move(components));
    connectSelectionModel();

    // Explicitly reset UI state
    ui->componentView->clearSelection();
    ui->actionEditComponent->setEnabled(false);
    ui->actionDeleteComponent->setEnabled(false);
}

bool MainWindow::createNewDatabase(const QString& fileName)
{
    if (QFileInfo::exists(fileName)) {
        auto response = QMessageBox::warning(
            this,
            tr("Overwrite Database"),
            tr("The selected database already exists.\n\n"
                "This will permanently delete all data.\n"
                "This action cannot be undone.\n\n"
                "Are you sure you want to continue?"),
            QMessageBox::Yes | QMessageBox::Cancel,
            QMessageBox::Cancel
        );

        if (response != QMessageBox::Yes)
            return false;

        if (!QFile::remove(fileName)) {
            QMessageBox::critical(this, tr("Error"),
                tr("Failed to delete the existing database file."));
            return false;
        }
    }

    if (!closeDatabase())
        return false;

    return openDatabase(fileName);
}

bool MainWindow::openExistingDatabase(const QString& fileName)
{
    if (!QFileInfo::exists(fileName)) {
        QMessageBox::critical(this, tr("Missing"),
            tr("Database not found."));
        return false;
    }

    if (!closeDatabase())
        return false;

    return openDatabase(fileName);
}

bool MainWindow::openDatabase(const QString& fileName)
{
    DbResult result;

    inventory_ = InventoryService::open(
        fileName.toStdString(),
        result
    );

    if (!inventory_) {
        QMessageBox::critical(
            this,
            tr("Database Error"),
            QString::fromStdString(result.message)
        );
        return false;
    }

    currentDatabasePath_ = fileName;
    enableDatabaseActions();
    updateWindowTitle(QFileInfo(fileName).fileName());
    statusBar()->showMessage(tr("Connected to %1").arg(fileName));

    reloadComponents();
    return true;
}

bool MainWindow::closeDatabase()
{
    if (!inventory_)
        return true;

    // Future: prompt for unsaved changes here

    inventory_.reset();   // 💥 closes DB via RAII
    currentDatabasePath_.clear();

    updateWindowTitle();
    statusBar()->showMessage(tr("Database disconnected"));

    disableDatabaseActions();
    clearComponentView();

    return true;
}

// --- UI state helpers ---

void MainWindow::enableDatabaseActions()
{
    ui->actionCloseDatabase->setEnabled(true);
    ui->actionAddComponent->setEnabled(true);
    ui->actionAddTestComponent->setEnabled(true);
}

void MainWindow::disableDatabaseActions()
{
    ui->actionCloseDatabase->setEnabled(false);
    ui->actionAddComponent->setEnabled(false);
    ui->actionEditComponent->setEnabled(false);
	ui->actionDeleteComponent->setEnabled(false);
	ui->actionAddTestComponent->setEnabled(false);
}

void MainWindow::updateWindowTitle(const QString& dbName)
{
    if (dbName.isEmpty())
        setWindowTitle(tr(kAppTitle));
    else
        setWindowTitle(tr("%1 – %2")
            .arg(tr(kAppTitle))
            .arg(dbName));
}

void MainWindow::connectSelectionModel()
{
    auto* sel = ui->componentView->selectionModel();
    if (!sel)
        return;

    connect(sel, &QItemSelectionModel::selectionChanged,
        this,
        [this, sel]() // capture sel explicitly
        {
            const bool hasSelection = sel->hasSelection();
            ui->actionEditComponent->setEnabled(hasSelection);
            ui->actionDeleteComponent->setEnabled(hasSelection);
        });
}
