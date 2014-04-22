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

int KRadix2::value(const QString &key)
{
//    return findNodeMatch(m_root, KStringUnicode(key)).value;
    return findNodeMatchIterative(&m_root, KStringUnicode(key))->value;
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

        for(int j = 1; ; j++){
            if(key[j] == nullUnicode && n.key[j] == nullUnicode) {
                return n; // key already exists
            }

            if(key[j] == nullUnicode) {
                return splitNode(n, j); // key smaller than node, split node
            }

            if(n.key[j] == nullUnicode) {
                return createNode(n, key.mid(j)); // go to child nodes
            }

            if(key[j] != n.key[j]) {
                return addNode(splitNode(n, j), key.mid(j)); // key diverging from node, split node
            }
        }
        curPos++;
    }

    return addNode(node, std::move(key));
}

Node2 &KRadix2::addNode(Node2& node, KStringUnicode key)
{
    node.childNodes.emplace_back(std::move(key));
    return node.childNodes.back();
}

Node2 &KRadix2::splitNode(Node2 &node, int pos)
{
    // New node with the last part of node as key
    Node2 newNode(node.key.mid(pos));
    newNode.value = node.value;
    newNode.childNodes = std::move(node.childNodes);

    // First part of node
    node.key = node.key.mid(0, pos);
    node.value = 0;
    node.childNodes.clear();
    node.childNodes.push_back(std::move(newNode));

    return node;
}

const Node2 &KRadix2::findNodeMatch(const Node2 &node, KStringUnicode key)
{
    int curPos = 0;
    for(const Node2& n : node.childNodes) {
        // Compare the first characters. Doing that first makes it easy to skip the current loop if it doesn't match.
        if(n.key[curPos] != key[curPos]) {
            continue;
        }
        curPos++;

        // Figure out where the characters start to diverge.
        while(key[curPos] != nullUnicode && n.key[curPos] == key[curPos]) {
            curPos++;
        }

        // If the key is at the end then we have a full match in the current node thus return the value.
        if(n.key[curPos] != nullUnicode) {
            return node;
        } else if(!n.childNodes.empty()) {
            key.midInternal(curPos); // Updates the current key object with the new length
            return findNodeMatch(n, std::move(key));
        } else {
            return n;
        }
    }
    return node;
}

Node2* KRadix2::findNodeMatchIterative(Node2* node, KStringUnicode key) const
{
    int i = 0;
    int offset = 0;
    Node2* currentNode = node;

    while(true) {
        int curPos = 0;
        Node2* n = &currentNode->childNodes[i];

        // Compare the first characters. Doing that first makes it easy to skip the current loop if it doesn't match.
        if(n->key[curPos] != key[curPos + offset]) {
            i++;
            continue;
        }
        curPos++;

        // Figure out where the characters start to diverge.
        while(key[curPos + offset] != nullUnicode && n->key[curPos] == key[curPos + offset]) {
            curPos++;
        }

        // If the key is at the end then we have a full match in the current node thus return the value.
        if(n->key[curPos] != nullUnicode) {
            return currentNode;
        } else if(key[curPos + offset] != nullUnicode && !n->childNodes.empty()) {
            offset += curPos;
            currentNode = n;
            i = 0;
            continue;
        } else {
            return n;
        }
        i++;
    }
    return currentNode;
}
