#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_ComponentEditDialog.h"
#include "SchemaManager.h"
#include "InventoryService.h"
#include "ComponentTableModel.h"
#include "ComponentEditDialog.h"
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

    // --- Setup central widget and layout for resizable table ---
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* layout = new QVBoxLayout(central);
    layout->addWidget(ui->componentView);
    layout->setContentsMargins(0, 0, 0, 0); // optional: remove outer margins

    // --- Table model ---
    componentModel_ = new ComponentTableModel(this); // db set later via InventoryService
    ui->componentView->setModel(componentModel_);

    // --- Stretch headers for nice resizing ---
    ui->componentView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    for (int i = 1; i < ui->componentView->model()->columnCount(); ++i)
        ui->componentView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);

    // --- Window & status ---
    updateWindowTitle();
    statusBar()->showMessage(tr("Ready"));
    disableDatabaseActions();

    // --- Connect actions ---
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onActionExit);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onActionAbout);
    connect(ui->actionNewDatabase, &QAction::triggered, this, &MainWindow::onActionNewDatabase);
    connect(ui->actionOpenDatabase, &QAction::triggered, this, &MainWindow::onActionOpenDatabase);
    connect(ui->actionCloseDatabase, &QAction::triggered, this, &MainWindow::onActionCloseDatabase);
	connect(ui->actionAddTestComponent, &QAction::triggered, this, &MainWindow::onActionAddTestComponent);
	connect(ui->actionDeleteComponent, &QAction::triggered, this, &MainWindow::onActionDeleteComponent);
    connect(ui->actionAddComponent, &QAction::triggered, this, &MainWindow::onActionAddComponent);
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
    if (!inventory_) return; // DB not open

    ComponentEditDialog dialog(*inventory_, this);

    // Start with a blank component
    Component c;
    dialog.setComponent(c);

    if (dialog.exec() == QDialog::Accepted) {
        c = dialog.component();

        DbResult result;
        if (!inventory_->components().addComponent(c, result)) {
            QMessageBox::critical(
                this,
                tr("Error"),
                QString::fromStdString(result.toString())
            );
            return;
        }

        reloadComponents();
        statusBar()->showMessage(tr("Component added"), 3000);
    }
}

void MainWindow::onActionDeleteComponent()
{
    if (!inventory_ || !componentModel_)
        return;

    QModelIndex idx = ui->componentView->currentIndex();
    if (!idx.isValid())
        return;

    int row = idx.row();
    int id = componentModel_->componentIdAt(row);

    DbResult result;
    if (!inventory_->components().deleteComponent(id, result)) {
        QMessageBox::critical(this, tr("Error"), QString::fromStdString(result.toString()));
        return;
    }

    reloadComponents();
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
    if (!inventory_->components().addComponent(c, result)) {
        QMessageBox::critical(this, tr("Error"), QString::fromStdString(result.toString()));
        return;
    }

    reloadComponents();
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
    if (!inventory_->components().listComponents(components, result)) {
        QMessageBox::critical(this, tr("Error"),
            QString::fromStdString(result.toString()));
        return;
    }

    // Load categories
    std::vector<Category> categories;
    inventory_->categories().listCategories(categories, result);

    std::unordered_map<int, QString> categoryMap;
    for (const auto& c : categories)
        categoryMap[c.id] = QString::fromStdString(c.name);

    // Load manufacturers
    std::vector<Manufacturer> manufacturers;
    inventory_->manufacturers().listManufacturers(manufacturers, result);

    std::unordered_map<int, QString> manufacturerMap;
    for (const auto& m : manufacturers)
        manufacturerMap[m.id] = QString::fromStdString(m.name);

    componentModel_->setCategoryLookup(std::move(categoryMap));
    componentModel_->setManufacturerLookup(std::move(manufacturerMap));
    componentModel_->setComponents(std::move(components));
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
    ui->actionDeleteComponent->setEnabled(true);
    ui->actionAddTestComponent->setEnabled(true);
}

void MainWindow::disableDatabaseActions()
{
    ui->actionCloseDatabase->setEnabled(false);
    ui->actionAddComponent->setEnabled(false);
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
