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

#include "kradix2.h"

#include <QDebug>

static const ushort nullUnicode = QChar('\0').unicode();

KRadix2::KRadix2(QObject *parent)
    : QObject(parent)
{

}

void KRadix2::insert(const QString &key, const int value)
{
    createNode(m_root, KStringUnicode(key)).value = value;
}

void KRadix2::printNodes()
{
    qDebug() << "--------------------------------------------------------";
    printNodes(m_root.childNodes, 0);
    qDebug() << "--------------------------------------------------------";
}

void KRadix2::printNodes(const std::vector<Node2>& nodes, int level)
{
    for(const Node2& n : nodes) {
        qDebug() << QString("- ").repeated(level) << "Key:" << n.key.toString() << "Value:" << n.value << "Num of childnodes:" << n.childNodes.size();
        if(!n.childNodes.empty()) {
            printNodes(n.childNodes, level + 1);
        }
    }
}

Node2 &KRadix2::createNode(Node2& node, KStringUnicode key)
{
    int curPos = 0;
    for(Node2& n : node.childNodes) {
        // Find matching node
        if(n.key[curPos] != key[curPos]) {
            continue;
        }

        //Node& n = node.childNodes[i];

        for(int j = 1; ; j++){
          if(key[j] == nullUnicode && n.key[j] == nullUnicode)
            return n; // key already exists
          if(key[j] == nullUnicode)
            return splitNode(n, j); // key smaller than node, split node
          if(n.key[j] == nullUnicode)
            return createNode(n, key.mid(0, j)); // go to child nodes
          if(key[j] != n.key[j]) {
            KStringUnicode u(key.mid(0, j));
            return addNode(splitNode(n, j), u); // key diverging from node, split node
          }
        }
        curPos++;
    }
    return addNode(node, key);
}

Node2 &KRadix2::addNode(Node2& node, KStringUnicode& key)
{
    node.childNodes.push_back(Node2(std::move(key)));
    return node.childNodes.back();
}

Node2 &KRadix2::splitNode(Node2 &node, int pos)
{
    // New node with the last part of node as key
    Node2 newNode(node.key.mid(pos));
    newNode.value = node.value;
    newNode.childNodes.swap(node.childNodes);

    // First part of node
    node.key = node.key.mid(0, pos);
    node.value = 0;
//    node.childNodes.clear();
    node.childNodes.push_back(std::move(newNode));

    return node;
}
