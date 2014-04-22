/*
    Copyright (C) 2014 Mark Gaiser <markg85@gmail.com>

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

#ifndef KRADIX2_H
#define KRADIX2_H

// Qt includes
#include <QObject>
#include <vector> // QVector doesn't support && insertion (rvalue reference)...

#include "kstringunicode.h"

struct Node2 {
    KStringUnicode key;
    int value;
    std::vector<Node2> childNodes;

    // no copy
//    Node2(const Node2&) = delete;

    // no assign
//    Node2& operator=(const Node2&) = delete;

    Node2(Node2&& n)
        : key(std::move(n.key))
        , value(n.value)
        , childNodes(std::move(n.childNodes))
    {}

    Node2(KStringUnicode&& n)
        : key(std::move(n))
        , value(0)
    {}

    Node2(QString data = "")
        : key(data)
        , value(0)
    {}
};

class KRadix2 : public QObject
{
    Q_OBJECT
public:
    explicit KRadix2(QObject* parent = 0);

    void insert(const QString& key, const int value);
    int value(const QString& key);
    void printNodes();

private:
    void printNodes(const std::vector<Node2>& nodes, int level);
    Node2& createNode(Node2& node, KStringUnicode key);
    Node2& addNode(Node2& node, KStringUnicode key);
    Node2& splitNode(Node2& node, int pos);
    const Node2& findNodeMatch(const Node2& node, KStringUnicode key);
    Node2* findNodeMatchIterative(Node2* node, KStringUnicode key) const;

private:
    Node2 m_root;
};

#endif // KRADIX2_H
