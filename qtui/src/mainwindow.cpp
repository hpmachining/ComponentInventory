#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "SchemaManager.h"
#include <QMessageBox>
#include <QStatusBar>
#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Component Inventory"));
    statusBar()->showMessage(tr("Ready"));
    disableDbActions();

    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onActionExit);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onActionAbout);
    connect(ui->actionNewDb, &QAction::triggered, this, &MainWindow::onActionNewDb);
    connect(ui->actionOpenDb, &QAction::triggered, this, &MainWindow::onActionOpenDb);
    connect(ui->actionCloseDb, &QAction::triggered, this, &MainWindow::onActionCloseDb);
}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::onActionNewDb()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Create New Database"),
        QDir::homePath() + "/inventory.db",
        tr("SQLite Database (*.db *.sqlite)")
    );

    if (fileName.isEmpty())
        return;

    if (QFileInfo::exists(fileName)) {
        QMessageBox::warning(this, tr("Exists"),
            tr("Database already exists."));
        return;
    }

    if (!closeDb())
        return;

    connectDb(fileName);
}

void MainWindow::onActionOpenDb()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open Database"),
        QDir::homePath(),
        tr("SQLite Database (*.db *.sqlite)")
    );

    if (fileName.isEmpty())
        return;

    if (!QFileInfo::exists(fileName)) {
        QMessageBox::critical(this, tr("Missing"),
            tr("Database not found."));
        return;
    }

    if (!closeDb())
        return;

    connectDb(fileName);
}

void MainWindow::onActionCloseDb()
{
    closeDb();
}

// --- Database lifecycle helpers ---

bool MainWindow::connectDb(const QString& fileName)
{
    DbResult result;
    db_ = std::make_unique<Database>(fileName.toStdString(), result);

    if (!db_->isOpen()) {
        QMessageBox::critical(this,
            tr("Database Error"),
            tr("Failed to open database:\n%1")
            .arg(QString::fromStdString(result.message)));
        db_.reset();
        return false;
    }

    SchemaManager schema(*db_);
    if (!schema.initialize(result)) {
        QMessageBox::critical(this,
            tr("Schema Error"),
            tr("Failed to initialize schema:\n%1")
            .arg(QString::fromStdString(result.message)));
        db_.reset();
        return false;
    }

    currentDbPath_ = fileName;
    setWindowTitle(tr("Inventory – %1")
        .arg(QFileInfo(fileName).fileName()));

    enableDbActions();
    statusBar()->showMessage(tr("Connected to %1").arg(fileName));
    return true;
}

bool MainWindow::closeDb()
{
    if (!db_)
        return true;

    // Future: unsaved changes check

    db_.reset();
    currentDbPath_.clear();

    setWindowTitle(tr("Inventory"));
    statusBar()->showMessage(tr("Database disconnected"));

    disableDbActions();
    // clearDbViews();

    return true;
}

// --- UI state helpers ---

void MainWindow::enableDbActions()
{
    ui->actionCloseDb->setEnabled(true);
}

void MainWindow::disableDbActions()
{
    ui->actionCloseDb->setEnabled(false);
}
