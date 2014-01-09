/*
 * Copyright (C) by Klaas Freitag <freitag@owncloud.com>
 * Copyright (C) by Krzesimir Nowak <krzesimir@endocode.com>
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

#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QUrl>

#include "wizard/owncloudwizardresultpage.h"
#include "wizard/owncloudwizardcommon.h"
#include "mirall/theme.h"

namespace Mirall
{

OwncloudWizardResultPage::OwncloudWizardResultPage()
  : QWizardPage(),
    _localFolder(),
    _remoteFolder(),
    _complete(false),
    _ui()
{
    _ui.setupUi(this);
    // no fields to register.

    setTitle(WizardCommon::subTitleTemplate().arg(tr("Everything set up!")));
    // required to show header in QWizard's modern style
    setSubTitle( QLatin1String(" ") );

    _ui.pbOpenLocal->setText(tr("Open Local Folder"));
    _ui.pbOpenLocal->setIcon(QIcon(":/mirall/resources/folder-sync.png"));
    _ui.pbOpenLocal->setIconSize(QSize(48, 48));
    _ui.pbOpenLocal->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(_ui.pbOpenLocal, SIGNAL(clicked()), SLOT(slotOpenLocal()));

    Theme* theme = Theme::instance();
    QIcon appIcon = theme->applicationIcon();
    _ui.pbOpenServer->setText(tr("Open %1").arg(theme->appNameGUI()));
    _ui.pbOpenServer->setIcon(appIcon.pixmap(48));
    _ui.pbOpenServer->setIconSize(QSize(48, 48));
    _ui.pbOpenServer->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(_ui.pbOpenServer, SIGNAL(clicked()), SLOT(slotOpenServer()));
    setupCustomization();
}

OwncloudWizardResultPage::~OwncloudWizardResultPage()
{}

void OwncloudWizardResultPage::setComplete(bool complete)
{
    _complete = complete;
    emit completeChanged();
}

bool OwncloudWizardResultPage::isComplete() const
{
    return _complete;
}

void OwncloudWizardResultPage::initializePage()
{
    const QString localFolder = wizard()->property("localFolder").toString();
    QString text;
    if( _remoteFolder == QLatin1String("/") || _remoteFolder.isEmpty() ) {
        text = tr("Your entire account is synced to the local folder <i>%1</i>")
                .arg(QDir::toNativeSeparators(localFolder));
    } else {
        text = tr("%1 folder <i>%1</i> is synced to local folder <i>%2</i>")
                .arg(Theme::instance()->appNameGUI())
                .arg(_remoteFolder).arg(QDir::toNativeSeparators(localFolder));
    }
    _ui.localFolderLabel->setText( text );

}

void OwncloudWizardResultPage::setRemoteFolder(const QString &remoteFolder)
{
    _remoteFolder = remoteFolder;
}

void OwncloudWizardResultPage::setupCustomization()
{
    // set defaults for the customize labels.
    _ui.topLabel->setText( QString::null );
    _ui.topLabel->hide();

    QVariant variant = Theme::instance()->customMedia( Theme::oCSetupResultTop );
    WizardCommon::setupCustomMedia( variant, _ui.topLabel );
}

void OwncloudWizardResultPage::slotOpenLocal()
{
    const QString localFolder = wizard()->property("localFolder").toString();
    QDesktopServices::openUrl(QUrl::fromLocalFile(localFolder));
}

void OwncloudWizardResultPage::slotOpenServer()
{
    QUrl url = field("OCUrl").toUrl();
    qDebug() << Q_FUNC_INFO << url;
    QDesktopServices::openUrl(url);
}

} // ns Mirall
