#pragma once

#include "Database.h"
#include "DbResult.h"
#include "ComponentTableModel.h"
#include <QMainWindow>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onActionExit();
    void onActionAbout();
    void onActionNewDb();
    void onActionOpenDb();
    void onActionCloseDb();

private:
    Ui::MainWindow* ui;
    
	// Database
    std::unique_ptr<Database> db_;
    QString currentDbPath_;

    // UI models
    ComponentTableModel* componentModel_ = nullptr;
    void clearComponentView();

    // Helpers
    bool createNewDb(const QString& fileName);
    bool openExistingDb(const QString& fileName);
    bool connectDb(const QString& fileName);
	bool closeDb();
    void enableDbActions();
    void disableDbActions();
    static constexpr const char* kAppTitle = "Component Inventory";
    void updateWindowTitle(const QString& dbName = QString());

};
