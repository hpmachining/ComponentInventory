#pragma once

#include <QAbstractTableModel>
#include <vector>
#include "Component.h"

class ComponentTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ComponentTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setComponents(std::vector<Component>&& comps);
    int componentIdAt(int row) const;

    void setCategoryLookup(std::unordered_map<int, QString> lookup);
    void setManufacturerLookup(std::unordered_map<int, QString> lookup);

private:
    std::vector<Component> components_;
    std::unordered_map<int, QString> categoryNames_;
    std::unordered_map<int, QString> manufacturerNames_;
};
