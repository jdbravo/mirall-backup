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

#include "mirall/account.h"
#include "mirall/mirallconfigfile.h"
#include "mirall/theme.h"

#include "wizard/owncloudwizard.h"
#include "wizard/owncloudsetuppage.h"
#include "wizard/owncloudhttpcredspage.h"
#include "wizard/owncloudshibbolethcredspage.h"
#include "wizard/owncloudadvancedsetuppage.h"
#include "wizard/owncloudwizardresultpage.h"

#include "QProgressIndicator.h"

#include <QtCore>
#include <QtGui>

#include <stdlib.h>

namespace Mirall
{

OwncloudWizard::OwncloudWizard(QWidget *parent)
    : QWizard(parent),
      _setupPage(new OwncloudSetupPage),
      _httpCredsPage(new OwncloudHttpCredsPage),
      _shibbolethCredsPage(new OwncloudShibbolethCredsPage),
      _advancedSetupPage(new OwncloudAdvancedSetupPage),
      _resultPage(new OwncloudWizardResultPage),
      _credentialsPage(0),
      _configFile(),
      _oCUser(),
      _setupLog(),
      _configExists(false)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setPage(WizardCommon::Page_ServerSetup, _setupPage);
    setPage(WizardCommon::Page_HttpCreds, _httpCredsPage);
    setPage(WizardCommon::Page_ShibbolethCreds, _shibbolethCredsPage);
    setPage(WizardCommon::Page_AdvancedSetup, _advancedSetupPage);
    setPage(WizardCommon::Page_Result, _resultPage);

    connect(this, SIGNAL(finished(int)), SIGNAL(basicSetupFinished(int)));

    // note: start Id is set by the calling class depending on if the
    // welcome text is to be shown or not.
    setWizardStyle( QWizard::ModernStyle );

    connect( this, SIGNAL(currentIdChanged(int)), SLOT(slotCurrentPageChanged(int)));
    connect( _setupPage, SIGNAL(determineAuthType(QString)), SIGNAL(determineAuthType(QString)));
    connect( _httpCredsPage, SIGNAL(connectToOCUrl(QString)), SIGNAL(connectToOCUrl(QString)));
    connect( _shibbolethCredsPage, SIGNAL(connectToOCUrl(QString)), SIGNAL(connectToOCUrl(QString)));
    connect( _advancedSetupPage, SIGNAL(createLocalAndRemoteFolders(QString, QString)),
             SIGNAL(createLocalAndRemoteFolders(QString, QString)));

    Theme *theme = Theme::instance();
    setWindowTitle( tr("%1 Connection Wizard").arg(theme->appNameGUI()));
    setWizardStyle(QWizard::ModernStyle);
    setPixmap( QWizard::BannerPixmap, theme->wizardHeaderBanner() );
    setPixmap( QWizard::LogoPixmap, theme->wizardHeaderLogo() );
    setOption( QWizard::NoBackButtonOnStartPage );
    setOption( QWizard::NoBackButtonOnLastPage );
    setOption( QWizard::NoCancelButton );
    setTitleFormat(Qt::RichText);
    setSubTitleFormat(Qt::RichText);
}

void OwncloudWizard::setAccount(Account *account)
{
    _account = account;
}

Account *OwncloudWizard::account() const
{
    return _account;
}

void OwncloudWizard::setMultipleFoldersExist(bool exist)
{
    _advancedSetupPage->setMultipleFoldersExist(exist);
}

QString OwncloudWizard::localFolder() const
{
    return(_advancedSetupPage->localFolder());
}

QString OwncloudWizard::ocUrl() const
{
    QString url = field("OCUrl").toString().simplified();
    return url;
}

void OwncloudWizard::enableFinishOnResultWidget(bool enable)
{
    _resultPage->setComplete(enable);
}

void OwncloudWizard::setRemoteFolder( const QString& remoteFolder )
{
    _advancedSetupPage->setRemoteFolder( remoteFolder );
    _resultPage->setRemoteFolder( remoteFolder );
}

void OwncloudWizard::successfulStep()
{
    const int id(currentId());

    switch (id) {
    case WizardCommon::Page_HttpCreds:
        _httpCredsPage->setConnected(true);
        break;

    case WizardCommon::Page_ShibbolethCreds:
        _shibbolethCredsPage->setConnected();
        break;

    case WizardCommon::Page_AdvancedSetup:
        _advancedSetupPage->directoriesCreated();
        break;

    case WizardCommon::Page_ServerSetup:
    case WizardCommon::Page_Result:
        qWarning("Should not happen at this stage.");
        break;
    }

    next();
}

void OwncloudWizard::setAuthType(WizardCommon::AuthType type)
{
  _setupPage->setAuthType(type);
  if (type == WizardCommon::Shibboleth) {
    _credentialsPage = _shibbolethCredsPage;
  } else {
    _credentialsPage = _httpCredsPage;
  }
  next();
}

// TODO: update this function
void OwncloudWizard::slotCurrentPageChanged( int id )
{
    qDebug() << "Current Wizard page changed to " << id;

    if( id == WizardCommon::Page_ServerSetup ) {
        emit clearPendingRequests();
        _setupPage->initializePage();
    }

    if( id == WizardCommon::Page_Result ) {
        disconnect(this, SIGNAL(finished(int)), this, SIGNAL(basicSetupFinished(int)));
        emit basicSetupFinished(QDialog::Accepted);
        appendToConfigurationLog( QString::null );
    }
}

void OwncloudWizard::displayError( const QString& msg )
{
    switch (currentId()) {
    case WizardCommon::Page_ServerSetup:
        _setupPage->setErrorString( msg );
        break;

    case WizardCommon::Page_HttpCreds:
        _httpCredsPage->setErrorString(msg);
        break;

    case WizardCommon::Page_AdvancedSetup:
        _advancedSetupPage->setErrorString(msg);
        break;
    }
}

void OwncloudWizard::appendToConfigurationLog( const QString& msg, LogType /*type*/ )
{
    _setupLog << msg;
    qDebug() << "Setup-Log: " << msg;
}

void OwncloudWizard::setOCUrl( const QString& url )
{
  _setupPage->setServerUrl( url );
}

void OwncloudWizard::setConfigExists( bool config )
{
    _configExists = config;
    _setupPage->setConfigExists( config );
    _httpCredsPage->setConfigExists(config);
    _advancedSetupPage->setConfigExists(config);
}

bool OwncloudWizard::configExists()
{
    return _configExists;
}

AbstractCredentials* OwncloudWizard::getCredentials() const
{
  if (_credentialsPage) {
    return _credentialsPage->getCredentials();
  }

  return 0;
}


} // end namespace
