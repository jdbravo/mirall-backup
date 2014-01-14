#include "owncloudnode.h"
#include <QVariant>

Node::Node(Type type, const QString &name, QDateTime lastModify, Node *_parent)
{
    this->type = type;
    this->name = name;
    this->parent = _parent;
    this->lastModify=lastModify;
    this->itemData << tr("Name") << "Last Modify";
}
Node::~Node()
{
    qDeleteAll(childrens);
}
bool Node::setData(int column, const QVariant &value)
 {
     if (column < 0 || column >= itemData.size())
         return false;

     itemData[column] = value;
     return true;
 }
Node *Node::child(int number)
{
    return childrens.value(number);
}
QString Node::path() {
    QString path=name;
    if (parent!=0) {
        path=parent->path()+"/"+name;
    }
    if (path.endsWith("/")) {
        path.chop(1);
    }
    if (!path.startsWith("/"))
        path="/"+path;
    return path;
}
QVariant Node::data(int column) const
{
    return itemData.value(column);
}

