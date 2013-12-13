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

KRadix::KRadix(QObject *parent)
    : QObject(parent)
    , m_nodes() // We "assume" that we don't need more then 32 (root) entries.
{

}

void KRadix::insert(const QString &key, const int value)
{
    if(!insertRootNodeIfNeeded(key, value)) {
        insert(m_nodes, key, value);
    }
}

int KRadix::value(const QString &key)
{
    return value(m_nodes, key);
}

void KRadix::printNodes()
{
    qDebug() << "--------------------------------------------------------";
    printNodes(m_nodes, 0);
    qDebug() << "--------------------------------------------------------";
}

void KRadix::printNodes(QVector<Node> nodes, int level)
{
    foreach(Node n, nodes) {
        qDebug() << QString("- ").repeated(level) << "Key:" << n.key << "Value:" << n.value << "Num of childnodes:" << n.childNodes.count();
        if(!n.childNodes.empty()) {
            printNodes(n.childNodes, level + 1);
        }
    }
}

bool KRadix::insertRootNodeIfNeeded(const QString &key, const int value)
{
    // This function simply checks if there is a root node matching the (start of) our current key. If there isn't it inserts a root node with this key.
    QChar firstChar = key.at(0);
    foreach(Node n, m_nodes) {
        if(n.key == key || n.key.startsWith(firstChar)) {
            return false;
        }
    }

    Node n;
    n.key = key;
    n.value = value;
    m_nodes << n;

    return true;
}

void KRadix::insert(QVector<Node> &nodes, const QString &key, const int value)
{
    // This function is doing the real complicated work. It iterates over all given nodes till there is no more match with the key.
    // At that point it splits the node it has to add the remainder part of the current key.

    int nodeCount = nodes.count();

    // Iterate over all given nodes
    for(int i = 0; i < nodeCount; i++) {
        // Now iterate over all values to find the node that matches most.
        Node& n = nodes[i];

        // As a quick test to find a potential partial match we check the first character
        if(n.key.startsWith(key.at(0))) {

            // Exact key match? Return since it's a duplicate.
            if(n.key == key) {
                return;
            }

            // Right, we have a node that matches the first character. So, how far does it match?
            int maxLength = qMin(n.key.length(), key.length());
            int startToDifferPosition = 0;

            // In this loop we can encounter a few different situations:
            // 1: There is a partial key match. In that case we need to split the current node
            // 2: There is a full key match, but we have more chars left. In that case we need to look at the childnodes
            for(; startToDifferPosition < maxLength; startToDifferPosition++) {

                // This is the partial key match case
                if(n.key.at(startToDifferPosition) != key.at(startToDifferPosition)) {
                    // At this point we start to differ.
                    QStringList newKeys = QStringList() << n.key.mid(0, startToDifferPosition) << n.key.mid(startToDifferPosition) << key.mid(startToDifferPosition);

                    // Create a few new nodes.
                    Node one = n;
                    Node two;

                    // Now update tempNode with it's new values. It's key is going to change and it's value is going to be reset unless this is the final length of the string and it's just splitted for another string
                    n.key = newKeys.at(0);
                    n.childNodes.clear();
                    if(!newKeys.at(1).isEmpty()) {
                        n.value = 0;
                        one.key = newKeys.at(1);
                        n.childNodes << one;
                    }


                    // Next update node two with the new key and value
                    two.key = newKeys.at(2);
                    two.value = value;

                    // Now insert the new nodes in our tempNode and we're done.
                    n.childNodes << two;
                    return;
                } else if (startToDifferPosition == maxLength - 1) {

                    QStringList newKeys = removeTextMatchFromBegin(n.key, key);
                    if(n.childNodes.isEmpty()) {
                        // Just insert a new node since it's empty anyway.
                        Node newNode;
                        newNode.key = newKeys.at(2);
                        newNode.value = value;
                        n.childNodes << newNode;
                    } else {
                        insert(n.childNodes, newKeys.at(2), value);
                    }
                    return;
                }
            }
        }
    }
}

int KRadix::value(QVector<Node> nodes, const QString& key)
{
    int nodeCount = nodes.count();
    for(int i = 0; i < nodeCount; i++) {
        Node& n = nodes[i];

        if(n.key == key) {
            return n.value;
        }

        QStringList keys = removeTextMatchFromBegin(n.key, key);

        if(!keys.at(0).isEmpty()) {
            return value(n.childNodes, keys.at(2));
        }
    }
    return 0;
}

QStringList KRadix::removeTextMatchFromBegin(const QString &one, const QString &two)
{
    // Figure out what does match
    int maxLength = qMin(one.length(), two.length());
    int startToDifferPosition = 0;
    for(; startToDifferPosition < maxLength; startToDifferPosition++) {
        if(one.at(startToDifferPosition) != two.at(startToDifferPosition)) {
            break;
        }
    }

    // This returns a list with 3 strings.
    // 1: the part that matches
    // 2: the part that didn't match from the "one" string
    // 3: the part that didn't match from the "two" string
    return QStringList() << one.mid(0, startToDifferPosition) << one.mid(startToDifferPosition) << two.mid(startToDifferPosition);
}
