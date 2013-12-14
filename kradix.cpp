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
    insert(m_nodes, key, value);
}

int KRadix::value(const QString &key)
{
    return value(m_nodes, QStringRef(&key));
}

void KRadix::matchPrefix(const QString &prefix)
{

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

void KRadix::insert(QVector<Node> &nodes, const QString &key, const int value)
{
    // This function is doing the real complicated work. It iterates over all given nodes till there is no more match with the key.
    // At that point it splits the node it has to add the remainder part of the current key.

    int nodeCount = nodes.count();
    bool potentialMatchFound = false;

    // Iterate over all given nodes
    for(int i = 0; i < nodeCount; i++) {
        // Now iterate over all values to find the node that matches most.
        Node& n = nodes[i];

//        qDebug() << "Testing first char:" << key.at(0) << "of key:" << key << "against key:";

        if(!n.key.startsWith(key.at(0))) {
            continue; // To be clear, this intentionally breaks the current loop iteration!
        }

        // As a quick test to find a potential partial match we check the first character
        potentialMatchFound = true;

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
                //QStringList newKeys = QStringList() << n.key.mid(0, startToDifferPosition) << n.key.mid(startToDifferPosition) << key.mid(startToDifferPosition);

                m_filtered[0] = n.key.mid(0, startToDifferPosition);
                m_filtered[1] = n.key.mid(startToDifferPosition);
                m_filtered[2] = key.mid(startToDifferPosition);

                // Create a few new nodes.
                Node one = n;
                Node two;

                // Now update tempNode with it's new values. It's key is going to change and it's value is going to be reset unless this is the final length of the string and it's just splitted for another string
                n.key = m_filtered[0];
                n.childNodes.clear();
                if(!m_filtered[1].isEmpty()) {
                    n.value = 0;
                    one.key = m_filtered[1];
                    n.childNodes << one;
                }


                // Next update node two with the new key and value
                two.key = m_filtered[2];
                two.value = value;

                // Now insert the new nodes in our tempNode and we're done.
                n.childNodes << two;
                return;
            } else if (startToDifferPosition == maxLength - 1) {
                //qDebug() << "startToDifferPosition == maxLength - 1" << n.key << key << "new key" << m_filtered[2].toString() << startToDifferPosition << key.mid(startToDifferPosition + 1);
                insert(n.childNodes, key.mid(startToDifferPosition + 1), value); // Why the +1? I don't now... -_- Have to figure that out some day.
                return;
            }
        }
    }

    if(!potentialMatchFound) {
//        qDebug() << "No potential node found, just add." << key;
        Node newNode;
        newNode.key = key;
        newNode.value = value;
        nodes << newNode;
    }
    return;
}

int KRadix::value(QVector<Node>& nodes, const QStringRef& key)
{
    int nodeCount = nodes.count();
    for(int i = 0; i < nodeCount; i++) {
        Node& n = nodes[i];

        if(n.key.at(0) != key.at(0)) {
            continue;
        }

        // Figure out what does match
        const int keyLength = key.length();
        const int nodeKeyLength = n.key.length();
        const int maxLength = qMin(nodeKeyLength, keyLength);
        int startToDifferPosition = 1; // Start at 1 since the first char is already checked.
        for(; startToDifferPosition < maxLength; startToDifferPosition++) {
            if(n.key.at(startToDifferPosition) != key.at(startToDifferPosition)) {
                break;
            }
        }

//        qDebug() << "Key:" << key << "matches with:" << startToDifferPosition << "from" << n.key;

        if(keyLength == nodeKeyLength) {
            return n.value;
        } else if(startToDifferPosition == nodeKeyLength) {
            return value(n.childNodes, key.right(startToDifferPosition));
        }
    }
    return 0;
}
