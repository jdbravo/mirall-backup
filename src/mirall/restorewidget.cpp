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


#include <QDebug>
#include <QLatin1Char>

#include "restorewidget.h"
#include "ui_restorewidget.h"

#include "mirall/networkjobs.h"
#include "account.h"
#include "mirall/owncloudfilesmodel.h"




namespace Mirall {

RestoreWidget::RestoreWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::RestoreWidget)
{
    _ui->setupUi(this);



    model = new OwncloudFilesModel();


    //_ui->treeViewRemote->header()->setStretchLastSection(false);
    _ui->treeViewRemote->header()->setResizeMode(QHeaderView::Stretch);

    connect(_ui->treeViewRemote,SIGNAL(pressed(QModelIndex)),this,SLOT(slotFilePressed(QModelIndex)));

    pI=new QProgressIndicator(this);
    _ui->spinnerLayout->addWidget(pI);
}

RestoreWidget::~RestoreWidget()
{
    delete _ui;
}

void RestoreWidget::slotFilePressed(QModelIndex index) {



    Node *parentNode = model->getItem(index);

    if (parentNode->getType()==Node::Dir&&parentNode->getChildrens()->isEmpty()) {
        pI->startAnimation();
        model->prepareToAdd();
        LsAllJob *job = new LsAllJob(AccountManager::instance()->account(),parentNode,this);
        connect(job,SIGNAL(fileListing(QVector<Node*>)),this,SLOT(slotLoadRemoteFiles(QVector<Node*>)));
        job->start();
        _ui->treeViewRemote->expand(index);
    }
    /*
    Node *node = new Node(Node::File,"prueba2",model->getItem(index));

    model->insertNode(index,node);
    _ui->treeViewRemote->expand(index);
    */

}

void RestoreWidget::loadFiles() {
    pI->startAnimation();
    Node *rootNode = new Node(Node::Dir, Account::backupPath(),QDateTime::currentDateTime()); //Invisible
    model->setRootNode(rootNode);
    LsAllJob *job = new LsAllJob(AccountManager::instance()->account(),rootNode,this);
    connect(job,SIGNAL(fileListing(QVector<Node*>)),this,SLOT(slotLoadRemoteFiles(QVector<Node*>)));
    job->start();
}

void RestoreWidget::slotLoadRemoteFiles(QVector<Node*> files) {
    if (!_ui->treeViewRemote->model()) {
        _ui->treeViewRemote->setModel(model);
    }


    model->added();
    pI->stopAnimation();
}




} // namespace Mirall
