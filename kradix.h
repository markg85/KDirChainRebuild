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

struct Node {
    QString key;
    QChar* data;
    int value;
    QVector<Node> childNodes;
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
    void printNodes(QVector<Node> nodes, int level);
    bool insertRootNodeIfNeeded(const QString &key, const int value);
    void insert(Node& node, const QString &key, const int value);
    void insertV2(Node& node, const QChar* key, const int value);
    int value(Node& node, const QChar* key);
    Node& findBestNodeMatch(Node& node, const QChar* key);
    Node& findNodeMatch(Node& node, const QChar* key);

private:
    Node m_root;
    QString m_filtered[3];
};

#endif // KRADIX_H
