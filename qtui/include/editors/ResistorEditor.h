#pragma once

#include <QWidget>
#include "IComponentEditor.h"
#include "ResistorManager.h"
#include "Database.h"

namespace Ui {
    class ResistorEditor;
}

class ResistorEditor : public QWidget, public IComponentEditor {
    Q_OBJECT

public:
    explicit ResistorEditor(Database& db, QWidget* parent = nullptr);
    ~ResistorEditor() override;

    QWidget* widget() override { return this; }
    void load(int componentId) override;
    bool save(int componentId, DbResult& result) override;

private:
    Database& db_;
    Ui::ResistorEditor* ui_;
};
