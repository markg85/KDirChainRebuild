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

class Shortcut : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant key READ key WRITE setKey NOTIFY keyChanged)
public:
    explicit Shortcut(QObject *parent = 0);

    void setKey(QVariant key);
    QVariant key() { return m_keySequence; }

    bool eventFilter(QObject *obj, QEvent *e);
    
signals:
    void keyChanged();
    void activated();
    void pressedAndHold();
    
public slots:

private:
    QKeySequence m_keySequence;
    bool m_keypressAlreadySend;
};

#endif // SHORTCUT_H
