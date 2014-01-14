#include "owncloudnode.h"

Node::Node(Type type, const QString &name, Node *_parent)
{
    this->type = type;
    this->name = name;
    parent = _parent;
}
Node::~Node()
{
    qDeleteAll(children);
}
bool Node::setData(int column, Node *value)
 {
     if (column < 0 || column >= children.size())
         return false;

     children[column] = value;
     return true;
 }
Node *Node::child(int number)
{
    return children.value(number);
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
