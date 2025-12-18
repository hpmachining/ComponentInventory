#pragma once

#include "Database.h"
#include "DbResult.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onActionExit();
    void onActionAbout();
    void onActionNewDb();
    void onActionOpenDb();
    void onActionCloseDb();

private:
    Ui::MainWindow* ui;
    std::unique_ptr<Database> db_;
    bool connectDb(const QString& fileName);
	bool closeDb();
    void enableDbActions();
    void disableDbActions();
    QString currentDbPath_;
};
