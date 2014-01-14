#include "owncloudfilesmodel.h"
#include "owncloudnode.h"

#include <QDebug>
#include <QIcon>


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
    Node *childNode = parentNode->getChildrens()->value(row);
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
    return parentNode->getChildrens()->count();
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

    int row = grandparentNode->getChildrens()->indexOf(parentNode);
    return createIndex(row, 0, parentNode);
}

QVariant OwncloudFilesModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::DecorationRole && role != Qt::CheckStateRole)
        return QVariant();

    Node *node = nodeFromIndex(index);

    if (!node)
        return QVariant();

    if (role == Qt::CheckStateRole)
    {
        if (index.column()==0) {
        return checkedFiles.contains(index) ? Qt::Checked : Qt::Unchecked;
        } else {
            return QVariant();
        }
    } else if (role==Qt::DisplayRole) {

        if (index.column() == 0) {

            return node->getName();
        } else if (index.column() == 1) {
            return node->getLastModify().toString();
        }
    } else if (role==Qt::DecorationRole) {
        if (index.column()==0) {
             if (node->getType()==Node::Dir) {
                 return QIcon(QLatin1String(":/mirall/resources/folder.png"));
             } else {
                 return QIcon(QLatin1String(":/mirall/resources/file.png"));
             }
        }

    }
    return QVariant();
}
QVariant OwncloudFilesModel::headerData(int section,
                                  Qt::Orientation orientation,
                                  int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            return tr("Name");
        } else if (section == 1) {
            return tr("Last Modify");
        }

    }
    return QVariant();
}

bool OwncloudFilesModel:: insertNode ( const QModelIndex & index, Node *value) {
    emit layoutAboutToBeChanged();
    Node *item = nodeFromIndex(index);
    if (item) {
        item->getChildrens()->append(value);
        emit  layoutChanged();
        return true;
    }
    return false;
}

bool OwncloudFilesModel::setData ( const QModelIndex & index, const QVariant &value, int role ) {
    if (role != Qt::EditRole && role != Qt::CheckStateRole)
             return false;


    if (role == Qt::CheckStateRole) {
        if (value == Qt::Checked) checkedFiles.insert(index);
                             else checkedFiles.remove(index);
        emit dataChanged(index, index);
        return true;
    }



    Node *item = nodeFromIndex(index);
    //qDebug()<<"the item is" << item->getName() << "and the colum is " << index.column();

    bool result = item->setData(index.column(), value);


    if (result)
     emit dataChanged(index, index);

    return result;

}

Qt::ItemFlags OwncloudFilesModel::flags(const QModelIndex& index) const {
    return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
}


