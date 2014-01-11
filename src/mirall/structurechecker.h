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

#ifndef STRUCTURECHECKER_H
#define STRUCTURECHECKER_H

#include <QStringList>
#include <QUrl>
#include <QNetworkReply>

#include "account.h"
#include "wizard/owncloudwizard.h"

namespace Mirall {


class StructureChecker : public QObject
{
    Q_OBJECT

public:
    StructureChecker(OwncloudWizard*);
    StructureChecker();
    void createStructure(const QString &first="");
    void setFolders(QStringList *);

private:
    QStringList _foldersStructure;
    OwncloudWizard* _ocWizard = NULL;
    Account *_account;
    QDir removeWebAddress(const QUrl&);

private slots:
    void slotAuthCheckReply(QNetworkReply *reply);
    void slotCreateRemoteFolderFinished(QNetworkReply *reply);

signals:
    void structureCreated();
};

}

// end namespace

#endif // STRUCTURECHECKER_H
