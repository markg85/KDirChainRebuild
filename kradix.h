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
    int value;
    QVector<Node> childNodes;
};

class KRadix : public QObject
{
    Q_OBJECT
public:
    explicit KRadix(QObject* parent = 0);

    void insert(const QString& key, const int value);
    bool containsKey(const QString& key);
    void findKey(const QString& key);

private:
    QVector<Node> m_nodes;
};

#endif // KRADIX_H
