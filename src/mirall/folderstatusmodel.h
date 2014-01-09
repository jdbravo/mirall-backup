/*
 * Copyright (C) by Klaas Freitag <freitag@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#ifndef FOLDERSTATUSMODEL_H
#define FOLDERSTATUSMODEL_H

#include <QStyledItemDelegate>
#include <QStandardItemModel>

namespace Mirall {

class FolderStatusModel : public QStandardItemModel
{
public:
    FolderStatusModel();
    virtual Qt::ItemFlags flags( const QModelIndex& ) const;
    QVariant data(const QModelIndex &index, int role) const;

};

class FolderStatusDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    public:
    FolderStatusDelegate();
    virtual ~FolderStatusDelegate();

    enum datarole { FolderAliasRole = Qt::UserRole + 100,
                    FolderPathRole,
                    FolderSecondPathRole,
                    FolderRemotePath,
                    FolderStatus,
                    FolderErrorMsg,
                    FolderSyncEnabled,
                    FolderStatusIconRole,

                    SyncProgressOverallPercent,
                    SyncProgressOverallString,
                    SyncProgressItemString,
                    AddProgressSpace,
                    WarningCount,
                    SyncRunning
                  };
    void paint( QPainter*, const QStyleOptionViewItem&, const QModelIndex& ) const;
    QSize sizeHint( const QStyleOptionViewItem&, const QModelIndex& ) const;
    bool editorEvent( QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option,
                      const QModelIndex& index );

private:
    bool _addProgressSpace;
};

} // namespace Mirall

#endif // FOLDERSTATUSMODEL_H
