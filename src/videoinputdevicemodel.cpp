#include "videoinputdevicemodel.h"

VideoInputDeviceModel::VideoInputDeviceModel(QObject *parent = 0)
    : QAbstractListModel(parent)
{}

VideoInputDeviceModel::~VideoInputDeviceModel() {}

int
VideoInputDeviceModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

int
VideoInputDeviceModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    /*
     * Only need one column.
     */
    return 1;
}

QVariant
VideoInputDeviceModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

QHash<int, QByteArray>
VideoInputDeviceModel::roleNames() const
{
    return QHash<int, QByteArray>();
}

QModelIndex
VideoInputDeviceModel::index(int row, int column, const QModelIndex &parent) const
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
VideoInputDeviceModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

Qt::ItemFlags
VideoInputDeviceModel::flags(const QModelIndex &index) const
{
    auto flags = QAbstractItemModel::flags(index) | Qt::ItemNeverHasChildren | Qt::ItemIsSelectable;
    if (!index.isValid()) {
        return QAbstractItemModel::flags(index);
    }
    return flags;
}

void
VideoInputDeviceModel::reset()
{
    beginResetModel();
    endResetModel();
}
