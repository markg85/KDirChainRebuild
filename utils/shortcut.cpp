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

#include "shortcut.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QDebug>

Shortcut::Shortcut(QObject *parent)
    : QObject(parent)
    , m_keys()
    , m_keypressAlreadySend(false)
    , m_mapFromString()
{
    qApp->installEventFilter(this);

    // Create a map from string to Qt::MouseButton.
    m_mapFromString.insert("NoButton",          Qt::NoButton);
    m_mapFromString.insert("LeftButton",        Qt::LeftButton);
    m_mapFromString.insert("RightButton",       Qt::RightButton);
    m_mapFromString.insert("MidButton",         Qt::MidButton);
    m_mapFromString.insert("MiddleButton",      Qt::MiddleButton);
    m_mapFromString.insert("BackButton",        Qt::BackButton);
    m_mapFromString.insert("XButton1",          Qt::XButton1);
    m_mapFromString.insert("ExtraButton1",      Qt::ExtraButton1);
    m_mapFromString.insert("ForwardButton",     Qt::ForwardButton);
    m_mapFromString.insert("ExtraButton2",      Qt::ExtraButton2);
    m_mapFromString.insert("TaskButton",        Qt::TaskButton);
    m_mapFromString.insert("ExtraButton3",      Qt::ExtraButton3);
    m_mapFromString.insert("ExtraButton4",      Qt::ExtraButton4);
    m_mapFromString.insert("ExtraButton5",      Qt::ExtraButton5);
    m_mapFromString.insert("ExtraButton6",      Qt::ExtraButton6);
    m_mapFromString.insert("ExtraButton7",      Qt::ExtraButton7);
    m_mapFromString.insert("ExtraButton8",      Qt::ExtraButton8);
    m_mapFromString.insert("ExtraButton9",      Qt::ExtraButton9);
    m_mapFromString.insert("ExtraButton10",     Qt::ExtraButton10);
    m_mapFromString.insert("ExtraButton11",     Qt::ExtraButton11);
    m_mapFromString.insert("ExtraButton12",     Qt::ExtraButton12);
    m_mapFromString.insert("ExtraButton13",     Qt::ExtraButton13);
    m_mapFromString.insert("ExtraButton14",     Qt::ExtraButton14);
    m_mapFromString.insert("ExtraButton15",     Qt::ExtraButton15);
    m_mapFromString.insert("ExtraButton16",     Qt::ExtraButton16);
    m_mapFromString.insert("ExtraButton17",     Qt::ExtraButton17);
    m_mapFromString.insert("ExtraButton18",     Qt::ExtraButton18);
    m_mapFromString.insert("ExtraButton19",     Qt::ExtraButton19);
    m_mapFromString.insert("ExtraButton20",     Qt::ExtraButton20);
    m_mapFromString.insert("ExtraButton21",     Qt::ExtraButton21);
    m_mapFromString.insert("ExtraButton22",     Qt::ExtraButton22);
    m_mapFromString.insert("ExtraButton23",     Qt::ExtraButton23);
    m_mapFromString.insert("ExtraButton24",     Qt::ExtraButton24);
    m_mapFromString.insert("AllButtons",        Qt::AllButtons);
    m_mapFromString.insert("MaxMouseButton",    Qt::MaxMouseButton);
    m_mapFromString.insert("WheelUp",           Qt::MaxMouseButton);    // TODO: Figure out right value.
    m_mapFromString.insert("WheelDown",         Qt::MaxMouseButton);    // TODO: Figure out right value.
    m_mapFromString.insert("WheelLeft",         Qt::MaxMouseButton);    // TODO: Figure out right value.
    m_mapFromString.insert("WheelRight",        Qt::MaxMouseButton);    // TODO: Figure out right value.
}

void Shortcut::setKeys(QStringList keys)
{
    qDebug() << "keys:" << keys;
    if(m_keys != keys) {
        m_keys = keys;

        foreach(QString k, keys) {
            QKeySequence seq(k);

            Key internalKey;

            if(seq.toString().isEmpty()) {
                if(!k.isEmpty()) {
                    // We "might" have a key with mouse bottons in it.
                    QStringList potentialMouseKeys = k.split("+", QString::SkipEmptyParts);
                    QStringList remainingKeys;
                    foreach(QString mouseKey, potentialMouseKeys) {
                        if(m_mapFromString.contains(mouseKey)) {
                            internalKey.mouseButtons.push_back(m_mapFromString.value(mouseKey));
                        } else {
                            remainingKeys << mouseKey;
                        }
                    }
                    internalKey.keys = QKeySequence(remainingKeys.join("+"));;
                }
            } else {
                // We have a valid key, no mouse buttons.
                internalKey.keys = seq;
            }

            m_keyPreCompute << internalKey;
        }

        emit keysChanged();
    }
}

QStringList Shortcut::keys()
{
    return m_keys;
}

bool Shortcut::eventFilter(QObject *obj, QEvent *e)
{
    if(e->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(e);
        QPoint angle = wheelEvent->angleDelta() / 8;

        if(angle.x() > 0) {
//            qDebug() << "Wheel left";
        } else if (angle.x() < 0) {
//            qDebug() << "Wheel right";
        }

        if(angle.y() > 0) {
//            qDebug() << "Wheel up";
        } else if (angle.y() < 0) {
//            qDebug() << "Wheel down";
        }

        // TODO
        // insert wheel event in m_currentPressedKeys.mouseButtons
    }


    if(e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease || e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonRelease) {

        // Construct the key sequence
        if(e->type() == QEvent::KeyPress) {
            // Add the currently pressed key to the vector.
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);
            if(!keyEvent->isAutoRepeat()) {
                int keyInt = keyEvent->modifiers() + keyEvent->key();
                m_currentPressedKeys.keys = QKeySequence(keyInt);
                m_keypressAlreadySend = false;
            }
        }

        // If we release a button, just reset the key sequence
        if(e->type() == QEvent::KeyRelease) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);
            if(!keyEvent->isAutoRepeat()) {
                m_currentPressedKeys.keys = QKeySequence();
                m_keypressAlreadySend = false;
            }
        }

        // Fill the mouse keys..
        if(e->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);

            // qDebug() << "Mouse press event:" << mouseEvent->type();

            if(!m_currentPressedKeys.mouseButtons.contains(mouseEvent->button())) {
                m_currentPressedKeys.mouseButtons.push_back(mouseEvent->button());
            }

        } else if (e->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
            int index = m_currentPressedKeys.mouseButtons.indexOf(mouseEvent->button());
            if(index >= 0) {
                m_currentPressedKeys.mouseButtons.remove(index);
                m_keypressAlreadySend = false;
            }
        }

        // Figure out which key matches and emit if we have a full match.
        foreach(Key k, m_keyPreCompute) {
            int mouseMatches = 0;
            bool keysFullyMatch = false;
            bool mouseFullyMatch = false;

            if(k.keys == m_currentPressedKeys.keys) {
                keysFullyMatch = true;
            }

            for(int i = 0; i < m_currentPressedKeys.mouseButtons.count(); i++) {
                if(!k.mouseButtons.contains(m_currentPressedKeys.mouseButtons[i])) {
                    break;
                }
                mouseMatches++;
            }

            if(mouseMatches == k.mouseButtons.count()) {
                // This is true, but can also be true if there are no matches!
                mouseFullyMatch = true;
            }

            if(keysFullyMatch && mouseFullyMatch) {
                if(!m_keypressAlreadySend) {
                    emit activated();
                }
                m_keypressAlreadySend = true;
                break;
            } else if (mouseFullyMatch && keysFullyMatch) {
                if(!m_keypressAlreadySend) {
                    emit activated();
                }
                m_keypressAlreadySend = true;
                break;
            } else if (keysFullyMatch && mouseFullyMatch) {
                if(!m_keypressAlreadySend) {
                    emit activated();
                }
                m_keypressAlreadySend = true;
                break;
            }
        }
    }
    return QObject::eventFilter(obj, e);
}
