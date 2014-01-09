#ifndef BACKUPFILESYSTEMMODEL_H
#define BACKUPFILESYSTEMMODEL_H

#include <QFileSystemModel>
#include <QStringList>
#include <QModelIndex>
#include <QVariant>
#include <QSet>
#include <QDebug>

class BackupFileSystemModel: public QFileSystemModel
{
    Q_OBJECT

private:
    QSet<QPersistentModelIndex> checklist;

public:
    BackupFileSystemModel(QObject *parent,QSet<QString>);

    ~BackupFileSystemModel();

    Qt::ItemFlags flags(const QModelIndex& index) const;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    QSet<QPersistentModelIndex> getChecklist();

};



#endif // BACKUPFILESYSTEMMODEL_H
