#pragma once

#include "Database.h"
#include "DbResult.h"
#include "ComponentTableModel.h"
#include <memory>
#include <QMainWindow>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class InventoryService;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
private:
    std::unique_ptr<InventoryService> inventory_;
protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onActionExit();
    void onActionAbout();
    void onActionNewDatabase();
    void onActionOpenDatabase();
    void onActionCloseDatabase();
    void onActionAddComponent();
    void onActionDeleteComponent();
    void onActionAddTestComponent();

private:
    Ui::MainWindow* ui;
    
	// Database
    //std::unique_ptr<Database> db_;
    QString currentDatabasePath_;

    // UI models
    ComponentTableModel* componentModel_ = nullptr;
    void clearComponentView();

    void reloadComponents();

    // Helpers
    bool createNewDatabase(const QString& fileName);
    bool openExistingDatabase(const QString& fileName);
    bool openDatabase(const QString& fileName);
	bool closeDatabase();
    void enableDatabaseActions();
    void disableDatabaseActions();
    static constexpr const char* kAppTitle = "Component Inventory";
    void updateWindowTitle(const QString& dbName = QString());

};
