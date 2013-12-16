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
{
    m_root.value = 0;
}

void KRadix::insert(const QString &key, const int value)
{
    insertV2(m_root, QString(key).data(), value);
//    insert(m_root, key, value);
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
    foreach(Node n, nodes) {
        qDebug() << QString("- ").repeated(level) << "Key:" << n.key << "Value:" << n.value << "Num of childnodes:" << n.childNodes.count();
        if(!n.childNodes.empty()) {
            printNodes(n.childNodes, level + 1);
        }
    }
}

void KRadix::insert(Node &node, const QString &key, const int value)
{
    // This function is doing the real complicated work. It iterates over all given nodes till there is no more match with the key.
    // At that point it splits the node it has to add the remainder part of the current key.

    int nodeCount = node.childNodes.count();
    bool potentialMatchFound = false;

    // Iterate over all given nodes
    for(int i = 0; i < nodeCount; i++) {
        // Now iterate over all values to find the node that matches most.
        Node& n = node.childNodes[i];

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
                n.data = n.key.data();
                n.childNodes.clear();
                if(!m_filtered[1].isEmpty()) {
                    n.value = 0;
                    one.key = m_filtered[1];
                    one.data = one.key.data();
                    n.childNodes << one;
                }


                // Next update node two with the new key and value
                two.key = m_filtered[2];
                two.data = two.key.data();
                two.value = value;

                // Now insert the new nodes in our tempNode and we're done.
                n.childNodes << two;
                return;
            } else if (startToDifferPosition == maxLength - 1) {
                //qDebug() << "startToDifferPosition == maxLength - 1" << n.key << key << "new key" << m_filtered[2].toString() << startToDifferPosition << key.mid(startToDifferPosition + 1);
                insert(n, key.mid(startToDifferPosition + 1), value); // Why the +1? I don't now... -_- Have to figure that out some day.
                return;
            }
        }
    }

    if(!potentialMatchFound) {
//        qDebug() << "No potential node found, just add." << key;
        Node newNode;
        newNode.key = key;
        newNode.data= newNode.key.data();
        newNode.value = value;
        node.childNodes << newNode;
    }
    return;
}

void KRadix::insertV2(Node &node, QChar* key, const int value)
{
    int keyPrefixMatch = 0;
    Node& n = findBestNodeMatch(node, key, &keyPrefixMatch);
    QChar* data = n.data;
    key += keyPrefixMatch;

    //qDebug() << "Key:" << QString(key) << "node key:" << n.key << "prefixMatchblabla:" << keyPrefixMatch;

    qDebug() << "Comparing key:" << n.key << "char:" << *key << "str key:" << QString(key) << "against data:" << *data << "str data:" << QString(data) << "prefixlength:" << keyPrefixMatch;

    // At this point we have the best matching node. All we need to figure out now is if we need to split our current node or can just simply insert to it;s childNodes

    // Figure out where the characters start to diverge.
    int startToDiverge = 0;
    while(*key != '\0' && *key == *data) {
        key++;
        data++;
        startToDiverge++;
    }

    // Exact match. No need to insert this one so just return.
    if(*key == '\0' && *data == '\0') {
        return;
    } else {
        // Now we could have a few different possibilities:
        // 1. Partial match where we need to split our current node in two.
        // 2. No match where we can just add this node.

        // Case 1. If we have devergence but not an ending key then we have to split our current node.
        if(startToDiverge > 0 && *key != '\0' && *data != '\0') {
//            qDebug() << "We have devergence!" << n.key << QString(n.data, startToDiverge) << QString(key) << QString(data);

            // Create a few new nodes.
            Node one = n;
            Node two;

            // Now update our current "n" node since we're splitting it.
            n.key = QString(n.data, startToDiverge); // The common key should be our new key.
            n.data = n.key.data();
            n.value = 0;
            n.childNodes.clear();

            // Next, update node one which was a copy of our "n" node. Update the parts that need to be changed.
            one.key = QString(data);
            one.data = one.key.data();

            // Then fill our new node
            two.key = QString(key);
            two.data = two.key.data();
            two.value = value;

            // Lastly, add out now nodex to the n node.
            n.childNodes << one << two;

        } else {
            // Case 2. No match, new node should be inserted.
            //qDebug() << "Insert new node. Value:" << value << "diverge:" << startToDiverge << "key.p" << *key << QString(key) << n.key;
            Node newNode;
            newNode.key = QString(key);
            newNode.data= newNode.key.data();
            newNode.value = value;
            n.childNodes << newNode;
        }
    }
}

// Find node that should be used to either add a child or split the node. Use this for insertion.
Node &KRadix::findBestNodeMatch(Node &node, const QChar *key, int* keyPrefixMatch)
{
    const int nodeCount = node.childNodes.count();
    for(int i = 0; i < nodeCount; i++) {
        // Compare the first characters. Doing that first makes it easy to skip the current loop if it doesn't match.

//        qDebug() << "Compare key:" << node.childNodes.at(i).key << "first char:" << *node.childNodes.at(i).data << "against" << *key;

        if(*node.childNodes.at(i).data != *key) {
            continue;
        }

        Node& n = node.childNodes[i];
        QChar* data = n.data;

        key++;
        data++;

        int numMatches = 0;
        // Figure out where the characters start to diverge.
        while(*key != '\0' && *key == *data) {
            key++;
            data++;
            numMatches++;
        }

//        qDebug() << "node key:" << n.key;

        // If the key is at the end then we have a full match in the current node thus return the value.
        if(*data == '\0' && !n.childNodes.isEmpty()) {
            *keyPrefixMatch += numMatches + 1;
//            qDebug() << "-- else if";
            return findBestNodeMatch(n, key, keyPrefixMatch);
        } else {
//            qDebug() << "-- else";
            return n;
        }
    }
    return node;
}

// Find an exact key match.
Node &KRadix::findNodeMatch(Node &node, const QChar *key)
{
    const int nodeCount = node.childNodes.count();
    for(int i = 0; i < nodeCount; i++) {
        // Compare the first characters. Doing that first makes it easy to skip the current loop if it doesn't match.

        if(*node.childNodes.at(i).data != *key) {
            continue;
        }

        Node& n = node.childNodes[i];
        QChar* data = n.data;

        key++;
        data++;

        // Figure out where the characters start to diverge.
        while(*key != '\0' && *key == *data) {
            key++;
            data++;
        }

        // If the key is at the end then we have a full match in the current node thus return the value.
        if(*data != '\0') {
            return node;
        } else if(!n.childNodes.isEmpty()) {
            return findNodeMatch(n, key);
        } else {
            return n;
        }
    }
    return node;
}
