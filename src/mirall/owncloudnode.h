#ifndef MIRALL_OWNCLOUDNODE_H
#define MIRALL_OWNCLOUDNODE_H

#include <QString>
#include <QList>
#include <QDateTime>
#include <QVector>

class Node : public QObject
{
    Q_OBJECT
public:
    enum Type { Dir, File };
    Node(Type type, const QString &name, QDateTime lastModify, Node *_parent = 0);
    ~Node();

    Node *parent;

    bool setData(int column, const QVariant &value);
    Node *child(int number);
    QString path();
    Node::Type getType( ) { return type; };
    QString getName() { return name; };
    QDateTime getLastModify() { return lastModify; };
    QList<Node*> *getChildrens() { return &childrens; };
    QVariant data(int column) const;

private:
    Type type;
    QString name;
    QDateTime lastModify;
    QList<Node*> childrens;
    QVector<QVariant> itemData;
};

#endif //MIRALL_OWNCLOUDNODE_H
