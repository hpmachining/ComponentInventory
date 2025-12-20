#include "ComponentTableModel.h"

ComponentTableModel::ComponentTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

void ComponentTableModel::setComponents(std::vector<Component>&& comps)
{
    beginResetModel();
    components_ = std::move(comps);
    endResetModel();
}

int ComponentTableModel::componentIdAt(int row) const
{
    if (row < 0 || row >= static_cast<int>(components_.size()))
        return -1;
    return components_[row].id;
}

int ComponentTableModel::rowCount(const QModelIndex&) const
{
    return static_cast<int>(components_.size());
}

int ComponentTableModel::columnCount(const QModelIndex&) const
{
    return 10;
}

QVariant ComponentTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const Component& c = components_[index.row()];

    switch (index.column()) {
    case 0: return c.id;
    case 1: return c.categoryId;
    case 2: return QString::fromStdString(c.partNumber);
    case 3: return c.manufacturerId;
    case 4: return QString::fromStdString(c.description);
    case 5: return QString::fromStdString(c.notes);
    case 6: return c.quantity;
    case 7: return QString::fromStdString(c.datasheetLink);
    case 8: return QString::fromStdString(c.createdOn);
    case 9: return QString::fromStdString(c.modifiedOn);
    default: return {};
    }
}

QVariant ComponentTableModel::headerData(
    int section,
    Qt::Orientation orientation,
    int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return {};

    switch (section) {
    case 0: return tr("ID");
    case 1: return tr("Category");
    case 2: return tr("Part Number");
    case 3: return tr("Manufacturer");
    case 4: return tr("Description");
    case 5: return tr("Notes");
    case 6: return tr("Quantity");
    case 7: return tr("Datasheet");
    case 8: return tr("Created");
    case 9: return tr("Modified");
    default: return {};
    }
}
