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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QStyledItemDelegate>

#include "mirall/progressdispatcher.h"

class QStandardItemModel;
class QListWidgetItem;

namespace Mirall {

namespace Ui {
class SettingsDialog;
}
class AccountSettings;
class ProtocolWidget;
class Application;
class FolderMan;
class ownCloudGui;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(ownCloudGui *gui, QWidget *parent = 0);
    ~SettingsDialog();

    void addAccount(const QString &title, QWidget *widget);
    void setGeneralErrors( const QStringList& errors );

public slots:
    void slotSyncStateChange(const QString& alias);
    void slotRefreshResultList();
    void showActivityPage();

protected:
    void reject();
    void accept();

private:
    Ui::SettingsDialog *_ui;
    AccountSettings *_accountSettings;
    QListWidgetItem *_accountItem;
    ProtocolWidget  *_protocolWidget;

    int _protocolIdx;
};

}

#endif // SETTINGSDIALOG_H
