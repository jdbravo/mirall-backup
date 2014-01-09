#include "backupfilesystemmodel.h"


BackupFileSystemModel::BackupFileSystemModel(QObject *parent,QSet<QString> folders)
    : QFileSystemModel(parent)
{
    foreach (const QString &stdValue, folders) {
        setData(index(stdValue), Qt::Checked, Qt::CheckStateRole);
    }
}
BackupFileSystemModel::~BackupFileSystemModel() {

}

QVariant BackupFileSystemModel::data(const QModelIndex& index, int role) const {
    if (role == Qt::CheckStateRole) return checklist.contains(index) ? Qt::Checked : Qt::Unchecked;
    return QFileSystemModel::data(index, role);
}

Qt::ItemFlags BackupFileSystemModel::flags(const QModelIndex& index) const {
    return QFileSystemModel::flags(index) | Qt::ItemIsUserCheckable;
}

bool BackupFileSystemModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (role == Qt::CheckStateRole) {
        if (value == Qt::Checked) checklist.insert(index);
                             else checklist.remove(index);
        emit dataChanged(index, index);
        return true;
    }
    return QFileSystemModel::setData(index, value, role);
}

QSet<QPersistentModelIndex> BackupFileSystemModel::getChecklist(){
    return checklist;
}
