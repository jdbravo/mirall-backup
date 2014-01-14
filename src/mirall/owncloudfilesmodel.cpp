#include "owncloudfilesmodel.h"
#include "owncloudnode.h"

#include <QDebug>


OwncloudFilesModel::OwncloudFilesModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    rootNode = 0;
}

OwncloudFilesModel::~OwncloudFilesModel()
{
    delete rootNode;
}

void OwncloudFilesModel::setRootNode(Node *node)
{
    delete rootNode;
    rootNode = node;
    reset();
}
Node *OwncloudFilesModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
            return static_cast<Node *>(index.internalPointer());
        } else {
            return rootNode;
    }
}
void OwncloudFilesModel::prepareToAdd() {
    emit layoutAboutToBeChanged();
}

void OwncloudFilesModel::added() {
    emit layoutChanged();
}
QModelIndex OwncloudFilesModel::index(int row, int column,
                                const QModelIndex &parent) const
{

    if (!rootNode || row < 0 || column < 0)
        return QModelIndex();
    Node *parentNode = nodeFromIndex(parent);
    Node *childNode = parentNode->children.value(row);
    if (!childNode)
        return QModelIndex();
    return createIndex(row, column, childNode);
}

Node *OwncloudFilesModel::nodeFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        Node *node = static_cast<Node *>(index.internalPointer());
        return node;
    } else {
        return rootNode;
    }
}
int OwncloudFilesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;
    Node *parentNode = nodeFromIndex(parent);
    if (!parentNode)
        return 0;
    return parentNode->children.count();
}
int OwncloudFilesModel::columnCount(const QModelIndex & /* parent */) const
{
    return 2;
}
QModelIndex OwncloudFilesModel::parent(const QModelIndex &child) const
{
    Node *node = nodeFromIndex(child);
    if (!node)
        return QModelIndex();

    Node *parentNode = node->parent;
    if (!parentNode)
        return QModelIndex();

    Node *grandparentNode = parentNode->parent;
    if (!grandparentNode)
        return QModelIndex();

    int row = grandparentNode->children.indexOf(parentNode);
    return createIndex(row, 0, parentNode);
}

QVariant OwncloudFilesModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    Node *node = nodeFromIndex(index);

    if (!node)
        return QVariant();

    if (index.column() == 0) {
        switch (node->getType()) {
        case Node::Dir:
             return tr("Dir");
        case Node::File:
            return tr("File");
        default:
            return tr("Other");
        }
    } else if (index.column() == 1) {
        return node->getName();
    }
    return QVariant();
}
QVariant OwncloudFilesModel::headerData(int section,
                                  Qt::Orientation orientation,
                                  int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            return tr("Type");
        } else if (section == 1) {
            return tr("Name");
        }
    }
    return QVariant();
}

bool OwncloudFilesModel:: insertNode ( const QModelIndex & index, Node *value) {
    emit layoutAboutToBeChanged();
    Node *item = nodeFromIndex(index);
    if (item) {
        item->children.append(value);
        emit  layoutChanged();
        return true;
    }
    return false;
}

bool OwncloudFilesModel::setData ( const QModelIndex & index, Node* value, int role ) {
    if (role != Qt::EditRole)
             return false;

    Node *item = nodeFromIndex(index);
    qDebug()<<"the item is" << item->getName() << "and the colum is " << index.column();

    bool result = item->setData(index.column(), value);

    if (result)
     emit dataChanged(index, index);

    return result;

}
