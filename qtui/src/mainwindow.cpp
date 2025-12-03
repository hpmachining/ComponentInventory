#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), model(nullptr)
{
    ui->setupUi(this);

    // Connect to SQLite (creates file if missing)
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("inventory.db");

    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", db.lastError().text());
        return;
    }

    // Create a simple table if it doesn't exist
     QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS components ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT, "
        "quantity INTEGER)");

    // Set up model
    model = new QSqlTableModel(this, db);
    model->setTable("components");
    model->select();

    // Bind to table view
    ui->tableView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}
