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

#include <QDebug>

#include "kradix.h"

const QChar cNull('\0');

KRadix::KRadix(QObject *parent)
    : QObject(parent)
{
    m_root.value = 0;
}

void KRadix::insert(const QString &key, const int value)
{
//  qDebug()<<"Insert"<<key<<", "<<value;

  //insert(m_root, key, value);
  createNode(m_root, QString(key).data()).value = value;
}

int KRadix::value(const QString &key)
{
    return findNodeMatch(m_root, key.constData()).value;
}

void KRadix::matchPrefix(const QString &prefix)
{

}

void KRadix::printNodes()
{
    qDebug() << "--------------------------------------------------------";
    printNodes(m_root.childNodes, 0);
    qDebug() << "--------------------------------------------------------";
}

void KRadix::printNodes(QVector<Node> nodes, int level)
{
    for(Node& n : nodes) {
        qDebug() << QString("- ").repeated(level) << "Key:" << n.key << "Value:" << n.value << "Num of childnodes:" << n.childNodes.count();
        if(!n.childNodes.empty()) {
            printNodes(n.childNodes, level + 1);
        }
    }
}

Node &KRadix::createNode(Node &node, const QChar *key)
{
    for(Node& n : node.childNodes) {
        const QChar* data = n.key.constData();
        // Find matching node
        if(*data != *key) {
            continue;
        }

        //Node& n = node.childNodes[i];

        for(int j = 1; ; j++){
          if(key[j] == cNull && data[j] == cNull)
            return n; // key already exists
          if(key[j] == cNull)
            return splitNode(n, j); // key smaller than node, split node
          if(data[j] == cNull)
            return createNode(n, &key[j]); // go to child nodes
          if(key[j] != data[j])
            return addNode(splitNode(n, j), &key[j]); // key diverging from node, split node
        }
    }

    return addNode(node, key);
}

// NOTE: Returns NEW node
Node &KRadix::addNode(Node& node, const QChar* key)
{
    Node newNode;
    newNode.key = QString(key);
    node.childNodes << newNode;
    return node.childNodes.last();
}

// NOTE: Returns the parent node
Node &KRadix::splitNode(Node &node, int pos)
{
    Node newNode = node;

    // Last part of node
    newNode.key = node.key.mid(pos);

    // First part of node
    node.key = node.key.mid(0, pos);
    node.value = 0;
    node.childNodes.clear();
    node.childNodes << newNode;

    return node;
}

// Find an exact key match.
Node &KRadix::findNodeMatch(Node &node, const QChar *key)
{
    for(Node& n : node.childNodes) {
        // Compare the first characters. Doing that first makes it easy to skip the current loop if it doesn't match.
        const QChar* data = n.key.constData();

        if(*data != *key) {
            continue;
        }

        key++;
        data++;

        // Figure out where the characters start to diverge.
        while(*key != cNull && *key == *data) {
            key++;
            data++;
        }

        // If the key is at the end then we have a full match in the current node thus return the value.
        if(*data != cNull) {
            return node;
        } else if(!n.childNodes.isEmpty()) {
            return findNodeMatch(n, key);
        } else {
            return n;
        }
    }
    return node;
}
