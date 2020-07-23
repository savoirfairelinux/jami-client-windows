#include "audioinputdevicemodel.h"

AudioInputDeviceModel::AudioInputDeviceModel(QObject *parent)
    : QAbstractListModel(parent)
{}

AudioInputDeviceModel::~AudioInputDeviceModel() {}

int
AudioInputDeviceModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

int
AudioInputDeviceModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    /*
     * Only need one column.
     */
    return 1;
}

QVariant
AudioInputDeviceModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

QHash<int, QByteArray>
AudioInputDeviceModel::roleNames() const
{
    return QHash<int, QByteArray>();
}

QModelIndex
AudioInputDeviceModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (column != 0) {
        return QModelIndex();
    }

    if (row >= 0 && row < rowCount()) {
        return createIndex(row, column);
    }
    return QModelIndex();
}

QModelIndex
AudioInputDeviceModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

Qt::ItemFlags
AudioInputDeviceModel::flags(const QModelIndex &index) const
{
    auto flags = QAbstractItemModel::flags(index) | Qt::ItemNeverHasChildren | Qt::ItemIsSelectable;
    if (!index.isValid()) {
        return QAbstractItemModel::flags(index);
    }
    return flags;
}

void
AudioInputDeviceModel::reset()
{
    beginResetModel();
    endResetModel();
}
