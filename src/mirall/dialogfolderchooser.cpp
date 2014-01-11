

#include <QTreeView>
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>

#include "folderman.h"
#include "dialogfolderchooser.h"
#include "ui_dialogfolderchooser.h"

namespace Mirall {

DialogFolderChooser::DialogFolderChooser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFolderChooser)
{
    ui->setupUi(this);
    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(false);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(slotFolderSelected()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));



    model = new QFileSystemModel();

    model->setRootPath(QDir::rootPath());

    model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);


    QTreeView *tree = ui->treeViewFolderChooser;
    tree->setModel(model);
    tree->setColumnHidden( 1, true );
    tree->setColumnHidden( 2, true );
    tree->setColumnHidden( 3, true );
    tree->setRootIndex(model->index(QDir::rootPath()));
    tree->setCurrentIndex(model->index(QDir::homePath()));
    tree->expand(model->index(QDir::homePath()));

    connect(tree,SIGNAL(pressed(QModelIndex)),this,SLOT(slotFolderPressed(QModelIndex)));

    pI=new QProgressIndicator(this);
    ui->spinnerLayout->addWidget(pI);


    sC=new StructureChecker();
}

void DialogFolderChooser::accept() {
     pI->startAnimation();

     FolderMan *folderMan = FolderMan::instance();
     Folder::Map map = folderMan->map();

     Folder::Map::const_iterator i = map.constBegin();
     QFileInfo selFile( QDir::fromNativeSeparators(selectedFolder) );
     QString userInput = selFile.canonicalFilePath();
     QStringList warnStrings;

     bool isOk=true;
     while( isOk && i != map.constEnd() ) {
       Folder *f = static_cast<Folder*>(i.value());
       QString folderDir = QDir( f->path() ).canonicalPath();
       if( folderDir.isEmpty() )
       {
         isOk = true;
         qDebug() << "Absolute path for folder: " << f->path() << " doesn't exist. Skipping.";
         i++;
         continue;
       }
       if( ! folderDir.endsWith(QLatin1Char('/')) ) folderDir.append(QLatin1Char('/'));

       qDebug() << "Checking local path: " << folderDir << " <-> " << userInput;
       if( QDir::cleanPath(f->path())  == QDir::cleanPath(userInput)  &&
               QDir::cleanPath(QDir(f->path()).canonicalPath()) == QDir(userInput).canonicalPath() ) {
         isOk = false;
         warnStrings.append( tr("The local path %1 is already an upload folder. Please pick another one!")
                            .arg(QDir::toNativeSeparators(userInput)) );
       }
       if( isOk && QDir::cleanPath(folderDir).startsWith(QDir::cleanPath(userInput)+'/') ) {
         qDebug() << "A already configured folder is child of the current selected";
         warnStrings.append( tr("An already configured folder is contained in the current entry."));
         isOk = false;
       }

       QString absCleanUserFolder = QDir::cleanPath(QDir(userInput).canonicalPath())+'/';
       if( isOk && QDir::cleanPath(folderDir).startsWith(absCleanUserFolder) ) {
           qDebug() << "A already configured folder is child of the current selected";
           warnStrings.append( tr("The selected folder is a symbolic link. An already configured"
                                 "folder is contained in the folder this link is pointing to."));
           isOk = false;
       }

       if( isOk && QDir::cleanPath(QString(userInput)).startsWith( QDir::cleanPath(folderDir)+'/') ) {
         qDebug() << "An already configured folder is parent of the current selected";
         warnStrings.append( tr("An already configured folder contains the currently entered folder."));
         isOk = false;
       }
       if( isOk && absCleanUserFolder.startsWith( QDir::cleanPath(folderDir)+'/') ) {
           qDebug() << "The selected folder is a symbolic link. An already configured folder is\n"
                       "the parent of the current selected contains the folder this link is pointing to.";
           warnStrings.append( tr("The selected folder is a symbolic link. An already configured folder "
                                 "is the parent of the current selected contains the folder this link is "
                                 "pointing to."));
           isOk = false;
       }

       i++;
     }

     if (isOk) {
     //create the structure
     connect(sC,SIGNAL(structureCreated()),this,SLOT(slotStructureCreated()));
     QStringList listFolders;
     listFolders.append(selectedFolder);
     sC->setFolders(&listFolders);
     sC->createStructure();
     } else {
         QMessageBox::warning(this,tr("Error adding the folder"),formatWarnings(warnStrings));
         pI->stopAnimation();
     }
}


QString DialogFolderChooser::formatWarnings(const QStringList &warnings) const
{
    QString ret;
    if (warnings.count() == 1) {
        ret = tr("<b>Warning:</b> ") + warnings.first();
    } else if (warnings.count() > 1) {
        ret = tr("<b>Warning:</b> ") + "<ul>";
        Q_FOREACH(QString warning, warnings) {
            ret += QString::fromLatin1("<li>%1</li>").arg(warning);
        }
        ret += "</ul>";
    }

    return ret;
}

QString DialogFolderChooser::getSelectedFolder() {
    return selectedFolder;
}

void DialogFolderChooser::slotStructureCreated() {
    qDebug()<<"structure created! ok";
    QDialog::accept();
}

void DialogFolderChooser::slotFolderSelected() {
    emit folderSelected(selectedFolder);
}

void DialogFolderChooser::slotFolderPressed(QModelIndex modelIndex) {
    selectedFolder=model->filePath(modelIndex);
    qDebug()<<"the user selected the folder " << selectedFolder;
    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(true);
}

DialogFolderChooser::~DialogFolderChooser()
{
    delete ui;
}

}
