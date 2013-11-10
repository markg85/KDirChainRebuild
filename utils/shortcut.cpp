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

#include "shortcut.h"
#include <QKeyEvent>
#include <QCoreApplication>
#include <QDebug>

Shortcut::Shortcut(QObject *parent)
    : QObject(parent)
    , m_keySequence()
    , m_keypressAlreadySend(false)
{
    qApp->installEventFilter(this);
}

void Shortcut::setKey(QVariant key)
{
    QKeySequence newKey = key.value<QKeySequence>();
    if(m_keySequence != newKey) {
        m_keySequence = key.value<QKeySequence>();
        emit keyChanged();
    }
}

bool Shortcut::eventFilter(QObject *obj, QEvent *e)
{
    if(e->type() == QEvent::KeyPress && !m_keySequence.isEmpty()) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);

        // Just mod keys is not enough for a shortcut, block them just by returning.
        if (keyEvent->key() >= Qt::Key_Shift && keyEvent->key() <= Qt::Key_Alt) {
            return QObject::eventFilter(obj, e);
        }

        int keyInt = keyEvent->modifiers() + keyEvent->key();

        if(!m_keypressAlreadySend && QKeySequence(keyInt) == m_keySequence) {
            m_keypressAlreadySend = true;
            emit activated();
        }
    }
    else if(e->type() == QEvent::KeyRelease) {
        m_keypressAlreadySend = false;
    }
    return QObject::eventFilter(obj, e);
}
