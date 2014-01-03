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

#ifndef SHORTCUT_H
#define SHORTCUT_H

#include <QVariant>
#include <QKeySequence>
#include <QVector>

struct Key {
    QVector<Qt::MouseButton> mouseButtons;
    QKeySequence keys;
};

class Shortcut : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList keys READ keys WRITE setKeys NOTIFY keysChanged)
public:
    explicit Shortcut(QObject *parent = 0);

    void setKeys(QStringList keys);
    QStringList keys();

    bool eventFilter(QObject *obj, QEvent *e);
    
signals:
    void keyChanged();
    void keysChanged();
    void activated();
    void pressedAndHold();
    
public slots:

private:
    QStringList m_keys;
    bool m_keypressAlreadySend;
    QHash<QString, Qt::MouseButton> m_mapFromString;
    QVector<Key> m_keyPreCompute;
    Key m_currentPressedKeys;
};

#endif // SHORTCUT_H
