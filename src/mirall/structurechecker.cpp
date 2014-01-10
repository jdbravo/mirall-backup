#include <QDir>
#include <QStringList>
#include <QDebug>
#include <QUrl>
#include <QDebug>

#include "structurechecker.h"
#include "account.h"
#include "networkjobs.h"

namespace Mirall {


StructureChecker::StructureChecker(OwncloudWizard *ocWizard) {
     _ocWizard=ocWizard;
}

void StructureChecker::createStructure(const QString &first) {

    qDebug() <<"Creating structure for "<< first;
    QString folder;
    if (!_foldersStructure.empty()) {

        if (first.isEmpty()) {
            folder=_foldersStructure.at(0);
        } else {
            folder=first;
        }

        EntityExistsJob *job = new EntityExistsJob(_ocWizard->account(), Account::concatDirPath(Account::davPath(),folder).toString(), this);
        connect(job, SIGNAL(exists(QNetworkReply*)), SLOT(slotAuthCheckReply(QNetworkReply*)));
        job->start();
    }
}



void StructureChecker::slotAuthCheckReply(QNetworkReply *reply)
{
    qDebug() << "response from slotAuthCheckReply";
    QString error;
    QNetworkReply::NetworkError errId = reply->error();
    QString remoteFolder=removeWebAddress(reply->url()).path();

    if( errId == QNetworkReply::NoError ) {
        qDebug() << "******** Remote folder found, all cool!";


        //if the folder is the last one in the list

        int position=_foldersStructure.indexOf(remoteFolder);

        if (position==-1) {
            if (remoteFolder.startsWith("/")) {
                position=_foldersStructure.indexOf(remoteFolder.mid(1));
            }
        }


        if (position==-1||position==_foldersStructure.count()-1) {
            qDebug() << "all directories created";
            emit structureCreated();
             _ocWizard->enableFinishOnResultWidget(true);
        } else {

            createStructure(_foldersStructure.at(position+1));
        }
    } else if( errId == QNetworkReply::ContentNotFoundError ) {
        if( remoteFolder.isEmpty() ) {
            error = tr("No remote folder specified!");

        } else {


            MkColJob *job = new MkColJob(_ocWizard->account(), remoteFolder, this);
            connect(job, SIGNAL(finished(QNetworkReply*)), SLOT(slotCreateRemoteFolderFinished(QNetworkReply *)));
            job->start();
        }
    } else {
        error = tr("Error: %1").arg(reply->errorString());
    }
    /*

    if( !ok ) {
        _ocWizard->displayError(error);
    }


    finalizeSetup( ok );
    */
}

void StructureChecker::slotCreateRemoteFolderFinished( QNetworkReply *reply )
{

    qDebug() << "** webdav mkdir request finished ";
    QString remoteFolder=removeWebAddress(reply->url()).path();
    if( ! reply->error() ) {
        _ocWizard->appendToConfigurationLog( tr("Remote folder %1 created successfully.").arg(remoteFolder));
        int position=_foldersStructure.indexOf(remoteFolder);
        if (position==-1) {
            if (remoteFolder.startsWith("/")) {
                position=_foldersStructure.indexOf(remoteFolder.mid(1));
            }
        }
        if (position==-1||position==_foldersStructure.count()-1) {
            qDebug() << "all directories created";
            emit structureCreated();
             _ocWizard->enableFinishOnResultWidget(true);
        } else {

            createStructure(_foldersStructure.at(position+1));
        }

    } else  {
        _ocWizard->appendToConfigurationLog( tr("Error creating the structure, cant create the dir %1, error: .").arg(remoteFolder,reply->errorString()));
    }
}

void StructureChecker::setFolders(QStringList *folders) {
    QDir backupDir = QDir(Account::backupPath());
    //We will generate the structure in order, ie: /home first then /home/user, etc
    foreach (const QString &value, *folders) {

        QStringListIterator partsIterator(value.split(QDir::separator()));
        QString path;
        _foldersStructure.append(backupDir.path());
        while (partsIterator.hasNext()) {
            QString tmpPath=partsIterator.next();
            if (!tmpPath.isEmpty()) {
                if (tmpPath.startsWith(QDir::separator()))
                    path.append(tmpPath);
                else
                    path.append(QDir::separator()+tmpPath);

                _foldersStructure.append(Account::concatDirPath(backupDir,path).path());
                qDebug() << "adding to the structure the dir " << Account::concatDirPath(backupDir,path).path();
            }
        }

    }
}

QDir StructureChecker::removeWebAddress(const QUrl &url) {
    QString dirPath = url.toString().replace(_ocWizard->account()->url().toString(),"");
    QString resourcePath = dirPath.replace(Account::davPath(),"");
    return QDir(resourcePath);
}

}
