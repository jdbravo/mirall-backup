#ifndef MIRALL_OWNCLOUDFILESMODEL_H
#define MIRALL_OWNCLOUDFILESMODEL_H

#include "owncloudfilesmodel.h"
#include "owncloudnode.h"

#include <QList>
#include <QAbstractItemModel>

class OwncloudFilesModel : public QAbstractItemModel
{
public:
    OwncloudFilesModel(QObject *parent = 0);
    ~OwncloudFilesModel();
    void setRootNode(Node *node);
    Node* getRootNode() { return rootNode; };
    QModelIndex index(int row, int column,
                      const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const;
    bool setData ( const QModelIndex & index, Node *value, int role );
    bool insertNode ( const QModelIndex & index, Node *value);
    Node* getItem(const QModelIndex &index) const;
    void prepareToAdd();
    void added();

private:
    Node *nodeFromIndex(const QModelIndex &index) const;
    Node *rootNode;
};

#endif MIRALL_OWNCLOUDFILESMODEL_H
