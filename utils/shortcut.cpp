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
    , m_keys()
    , m_keypressAlreadySend(false)
{
    qApp->installEventFilter(this);
}

void Shortcut::setKeys(QStringList keys)
{
    qDebug() << "keys:" << keys;
    if(m_keys != keys) {
        m_keys = keys;
        emit keysChanged();
    }
}

QStringList Shortcut::keys()
{
    return m_keys;
}

bool Shortcut::eventFilter(QObject *obj, QEvent *e)
{
    foreach (const QString& key, m_keys) {
        const QKeySequence sequence(key);
        if(e->type() == QEvent::KeyPress && !sequence.isEmpty()) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);

            // Just mod keys is not enough for a shortcut, block them just by returning.
            if (keyEvent->key() >= Qt::Key_Shift && keyEvent->key() <= Qt::Key_Alt) {
                return QObject::eventFilter(obj, e);
            }

            int keyInt = keyEvent->modifiers() + keyEvent->key();

            if(!m_keypressAlreadySend && QKeySequence(keyInt) == sequence) {
                m_keypressAlreadySend = true;
                emit activated();
            }
        }
        else if(e->type() == QEvent::KeyRelease) {
            m_keypressAlreadySend = false;
        }
    }
    return QObject::eventFilter(obj, e);
}
