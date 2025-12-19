#pragma once

#include <QAbstractTableModel>
#include <vector>
#include "Component.h"
#include "Database.h"

class ComponentTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ComponentTableModel(Database* db, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void reload();  // refresh from DB

private:
    Database* db_;                    // non-owning
    std::vector<Component> components_;
};
