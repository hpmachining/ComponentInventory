#include "ComponentTableModel.h"
#include "ComponentManager.h"
#include "DbResult.h"

ComponentTableModel::ComponentTableModel(Database* db, QObject* parent)
    : QAbstractTableModel(parent), db_(db)
{
    reload();
}

void ComponentTableModel::reload()
{
    beginResetModel();

    components_.clear();

    if (db_) {
        ComponentManager mgr(*db_);
        DbResult result;
        mgr.listComponents(components_, result);
        // If needed later: handle result.error here
    }

    endResetModel();
}

int ComponentTableModel::rowCount(const QModelIndex&) const
{
    return static_cast<int>(components_.size());
}

int ComponentTableModel::columnCount(const QModelIndex&) const
{
    return 10; // matches SELECT in listComponents()
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
