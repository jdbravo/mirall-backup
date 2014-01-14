#ifndef MIRALL_OWNCLOUDNODE_H
#define MIRALL_OWNCLOUDNODE_H

#include <QString>
#include <QList>

class Node
{
public:
    enum Type { Dir, File };
    Node(Type type, const QString &name = "", Node *_parent = 0);
    ~Node();

    Node *parent;
    QList<Node *> children;
    bool setData(int column, Node *value);
    Node *child(int number);
    QString path();
    Node::Type getType( ) { return type; };
    QString getName() { return name; };
private:
    Type type;
    QString name;

};

#endif //MIRALL_OWNCLOUDNODE_H
