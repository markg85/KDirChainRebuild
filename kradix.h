/*
    Copyright (C) 2013 Mark Gaiser <markg85@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef KRADIX_H
#define KRADIX_H

// Qt includes
#include <QObject>
#include <QVector>
#include <vector>

struct Node {
    QString key;
    int value;
    std::vector<Node> childNodes;

    Node(QString data = "")
        : key(data)
    {
    }
};

class KRadix : public QObject
{
    Q_OBJECT
public:
    explicit KRadix(QObject* parent = 0);

    void insert(const QString& key, const int value);
    int value(const QString& key);
    void matchPrefix(const QString& prefix);
    void printNodes();

private:
    void printNodes(std::vector<Node> nodes, int level);
    int value(Node& node, const QChar* key);
    Node& createNode(Node& node, const QChar* key);
    Node& addNode(Node& node, const QChar* key);
    Node& splitNode(Node& node, int pos);
    Node& findNodeMatch(Node& node, const QChar* key);
    Node& findNodeMatchIterative(Node& node, const QChar* key);

private:
    Node m_root;
};

#endif // KRADIX_H
