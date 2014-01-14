/*
 * Copyright (C) by Daniel Molkentin <danimo@owncloud.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#ifndef MIRALL_RESTOREWIDGET_H
#define MIRALL_RESTOREWIDGET_H

#include <QWidget>
#include <QStringList>
#include <QVector>

#include <owncloudfilesmodel.h>
#include "QProgressIndicator.h"


namespace Mirall {

namespace Ui {
class RestoreWidget;
}

class RestoreWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RestoreWidget(QWidget *parent = 0);
    ~RestoreWidget();
    void loadFiles();


    Ui::RestoreWidget *_ui;
private:
    OwncloudFilesModel *model;
    QProgressIndicator *pI;

private slots:
    void slotLoadRemoteFiles(QVector<Node*>);
    void slotFilePressed(QModelIndex);

};


} // namespace Mirall
#endif // MIRALL_RESTOREWIDGET_H
