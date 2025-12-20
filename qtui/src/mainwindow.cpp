#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "SchemaManager.h"
#include "InventoryService.h"
#include "ComponentTableModel.h"
#include "DevDataSeeder.h"
#include <QMessageBox>
#include <QStatusBar>
#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    updateWindowTitle();
    statusBar()->showMessage(tr("Ready"));
    disableDatabaseActions();

    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onActionExit);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onActionAbout);
    connect(ui->actionNewDatabase, &QAction::triggered, this, &MainWindow::onActionNewDatabase);
    connect(ui->actionOpenDatabase, &QAction::triggered, this, &MainWindow::onActionOpenDatabase);
    connect(ui->actionCloseDatabase, &QAction::triggered, this, &MainWindow::onActionCloseDatabase);
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

// --- Database lifecycle helpers ---

void MainWindow::clearComponentView()
{
    if (ui->tableViewComponents->model()) {
        ui->tableViewComponents->setModel(nullptr); // disconnect model
    }

    delete componentModel_;  // free the old model
    componentModel_ = nullptr;
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
    statusBar()->showMessage(tr("Connected to %1").arg(fileName));
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
}

void MainWindow::disableDatabaseActions()
{
    ui->actionCloseDatabase->setEnabled(false);
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
