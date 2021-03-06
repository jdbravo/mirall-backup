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

#include <QDir>
#include <QFileDialog>
#include <QUrl>
#include <QTimer>
#include <QTreeView>

#include "QProgressIndicator.h"

#include "wizard/owncloudwizard.h"
#include "wizard/owncloudwizardcommon.h"
#include "wizard/owncloudadvancedsetuppage.h"
#include "mirall/account.h"
#include "mirall/theme.h"
#include "mirall/mirallconfigfile.h"
#include "creds/abstractcredentials.h"
#include "mirall/backupfilesystemmodel.h"

namespace Mirall
{

OwncloudAdvancedSetupPage::OwncloudAdvancedSetupPage()
  : QWizardPage(),
    _ui(),
    _checking(false),
    _created(false),
    _configExists(false),
    _multipleFoldersExist(false),
    _progressIndi(new QProgressIndicator (this)),
    _oldLocalFolder(),
    _remoteFolder()
{
    _ui.setupUi(this);

    Theme *theme = Theme::instance();
    setTitle(WizardCommon::titleTemplate().arg(tr("Connect to %1").arg(theme->appNameGUI())));
    setSubTitle(WizardCommon::subTitleTemplate().arg(tr("Setup local folder options")));

    registerField( QLatin1String("OCSyncFromScratch"), _ui.cbSyncFromScratch);

    _ui.resultLayout->addWidget( _progressIndi );
    stopSpinner();
    setupCustomization();

    //Jdbravo: We will not use the pbSelectLocalFolder any more, instead we will use the treeView
    //connect( _ui.pbSelectLocalFolder, SIGNAL(clicked()), SLOT(slotSelectFolder()));
    setButtonText(QWizard::NextButton, tr("Connect..."));

    QTreeView *tree = _ui.wizardTreeView;
    QSet<QString> folders;
    model = new BackupFileSystemModel(this,folders);

    model->setRootPath(QDir::rootPath());

    model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);



    tree->setModel(model);
    tree->setColumnHidden( 1, true );
    tree->setColumnHidden( 2, true );
    tree->setColumnHidden( 3, true );
    tree->setRootIndex(model->index(QDir::rootPath()));
    tree->setCurrentIndex(model->index(QDir::homePath()));
    tree->expand(model->index(QDir::homePath()));


}

void OwncloudAdvancedSetupPage::setupCustomization()
{
    // set defaults for the customize labels.
    _ui.topLabel->hide();
    _ui.bottomLabel->hide();

    Theme *theme = Theme::instance();
    QVariant variant = theme->customMedia( Theme::oCSetupTop );
    if( !variant.isNull() ) {
        WizardCommon::setupCustomMedia( variant, _ui.topLabel );
    }

    variant = theme->customMedia( Theme::oCSetupBottom );
    WizardCommon::setupCustomMedia( variant, _ui.bottomLabel );
}

bool OwncloudAdvancedSetupPage::isComplete() const
{
    return !_checking;
}

void OwncloudAdvancedSetupPage::initializePage()
{
    WizardCommon::initErrorLabel(_ui.errorLabel);

    _checking  = false;
    _multipleFoldersExist = false;
    _oldLocalFolder = localFolder();

    // call to init label
    updateStatus();

    // ensure "next" gets the focus, not obSelectLocalFolder
    QTimer::singleShot(0, wizard()->button(QWizard::NextButton), SLOT(setFocus()));
}

// Called if the user changes the user- or url field. Adjust the texts and
// evtl. warnings on the dialog.
void OwncloudAdvancedSetupPage::updateStatus()
{
    //I don't what to show this for backups
    /*
    const QString locFolder = localFolder();
    // check if the local folder exists. If so, and if its not empty, show a warning.
    QString t;


    _ui.pbSelectLocalFolder->setText(QDir::toNativeSeparators(locFolder));
    if (dataChanged()) {
        if( _remoteFolder.isEmpty() || _remoteFolder == QLatin1String("/") ) {
            t = tr("Your entire account will be synced to the local folder '%1'.")
                .arg(QDir::toNativeSeparators(locFolder));
        } else {
            t = tr("%1 folder '%2' is synced to local folder '%3'")
                .arg(Theme::instance()->appName()).arg(_remoteFolder)
                .arg(QDir::toNativeSeparators(locFolder));
        }

        if ( _multipleFoldersExist ) {
            t += tr("<p><small><strong>Warning:</strong> You currently have multiple folders "
                    "configured. If you continue with the current settings, the folder configurations "
                    "will be discarded and a single root folder sync will be created!</small></p>");
        }

        const bool dirNotEmpty(QDir(locFolder).entryList(QDir::AllEntries | QDir::NoDotAndDotDot).count() > 0);
        if(dirNotEmpty) {
            t += tr("<p><small><strong>Warning:</strong> The local directory is not empty. "
                    "Pick a resolution!</small></p>");
        }
        //_ui.resolutionWidget->setVisible(dirNotEmpty);
        _ui.resolutionWidget->setVisible(false);
    } else {
        _ui.resolutionWidget->setVisible(false);
    }

    _ui.syncModeLabel->setText(t);

    wizard()->resize(wizard()->sizeHint());
    */

    _ui.resolutionWidget->setVisible(false);
    _ui.syncModeLabel->setText("");
}

bool OwncloudAdvancedSetupPage::dataChanged()
{
    OwncloudWizard* ocWizard(dynamic_cast< OwncloudWizard* >(wizard()));
    Account *oldAccount = AccountManager::instance()->account();

    if (!ocWizard || !oldAccount) {
        return false;
    }

    const QString url(field("OCUrl").toString());
    AbstractCredentials* newCredentials(ocWizard->getCredentials());
    AbstractCredentials* oldCredentials(oldAccount->credentials());
    const bool differentCreds(oldCredentials->changed(newCredentials));
    delete newCredentials;
    //Jdbravo: We will not use the pbSelectLocalFolder any more, instead we will use the treeView
    const QString newLocalFolder("");
    //const QString newLocalFolder(QDir::toNativeSeparators(_ui.pbSelectLocalFolder->text()));
    const QString oldLocalFolder(QDir::toNativeSeparators(_oldLocalFolder));

    return ((url != oldAccount->url().toString()) || differentCreds || (oldLocalFolder != newLocalFolder));
}

void OwncloudAdvancedSetupPage::startSpinner()
{
    _ui.resultLayout->setEnabled(true);
    _progressIndi->setVisible(true);
    _progressIndi->startAnimation();
}

void OwncloudAdvancedSetupPage::stopSpinner()
{
    _ui.resultLayout->setEnabled(false);
    _progressIndi->setVisible(false);
    _progressIndi->stopAnimation();
}

int OwncloudAdvancedSetupPage::nextId() const
{
    return WizardCommon::Page_Result;
}

QString OwncloudAdvancedSetupPage::localFolder() const
{
    QString folder = wizard()->property("localFolder").toString();
    return folder;
}
QStringList OwncloudAdvancedSetupPage::localFolders()
{
    return _localFolders;
}

bool OwncloudAdvancedSetupPage::validatePage()
{
    //I don't know where exactly get the list of checked folders, but here is working fine
    QSet<QPersistentModelIndex> selectedFoldersIndex = model->getChecklist();
    _localFolders.clear();
    if (!selectedFoldersIndex.empty()) {
        foreach (const QPersistentModelIndex &value, selectedFoldersIndex) {
            QString folderPath=model->filePath(value);
            _localFolders.append(folderPath);
            qDebug()<<"local folder selected " << folderPath;
        }
        wizard()->setProperty("localFolders", _localFolders);
    }


    if(!_created) {
        setErrorString(QString::null);
        _checking = true;
        startSpinner();
        emit completeChanged();

        //emit createLocalAndRemoteFolders(localFolder(), _remoteFolder);
        emit createRemoteFolders(&_localFolders);
        return false;
    } else {
        // connecting is running
        _checking = false;
        emit completeChanged();
        stopSpinner();
        return true;
    }
}

void OwncloudAdvancedSetupPage::setErrorString( const QString& err )
{
    if( err.isEmpty()) {
        _ui.errorLabel->setVisible(false);
    } else {
        _ui.errorLabel->setVisible(true);
        _ui.errorLabel->setText(err);
    }
    _checking = false;
    emit completeChanged();
}

void OwncloudAdvancedSetupPage::directoriesCreated()
{
    _checking = false;
    _created = true;
    stopSpinner();
    emit completeChanged();
}

void OwncloudAdvancedSetupPage::setRemoteFolder( const QString& remoteFolder )
{
    if( !remoteFolder.isEmpty() ) {
        _remoteFolder = remoteFolder;
    }
}

void OwncloudAdvancedSetupPage::setMultipleFoldersExist(bool exist)
{
    _multipleFoldersExist = exist;
}

void OwncloudAdvancedSetupPage::setConfigExists(bool config)
{
    _configExists = config;

    if (config == true) {
        setSubTitle(WizardCommon::subTitleTemplate().arg(tr("Update advanced setup")));
    }
}

} // ns Mirall
