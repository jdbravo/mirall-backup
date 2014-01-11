#ifndef DIALOGFOLDERCHOOSER_H
#define DIALOGFOLDERCHOOSER_H

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

#endif // DIALOGFOLDERCHOOSER_H
