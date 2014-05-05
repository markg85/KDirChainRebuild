/*
    The MIT License (MIT)

    Copyright (C) 2014 Mark Gaiser <markg85@gmail.com>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
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
