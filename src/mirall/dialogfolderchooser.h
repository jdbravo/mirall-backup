/*
 * Copyright (C) by Jose Bravo <jbravo@colombiahosting.com.co>
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

#ifndef MIRALL_DIALOGFOLDERCHOOSER_H
#define MIRALL_DIALOGFOLDERCHOOSER_H

#include <QDialog>
#include <QFileSystemModel>
#include <QModelIndex>

#include "QProgressIndicator.h"
#include "structurechecker.h"

namespace Mirall {

namespace Ui {
class DialogFolderChooser;
}

class DialogFolderChooser : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogFolderChooser(QWidget *parent = 0);
    ~DialogFolderChooser();
    QString getSelectedFolder();
    
private:
    Ui::DialogFolderChooser *ui;
    QFileSystemModel *model;
    QString selectedFolder;
    QProgressIndicator *pI;
    StructureChecker *sC;
    QString formatWarnings(const QStringList &) const;


private slots:
    void slotFolderPressed(QModelIndex);
    void slotFolderSelected();
    void slotStructureCreated();
public slots:
    virtual void accept ();

signals:
    void folderSelected(QString path);
};

} //namespace mirall

#endif // MIRALL_DIALOGFOLDERCHOOSER_H
