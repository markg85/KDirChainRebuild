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

#include "urlundoredo.h"
#include <QDebug>

UrlUndoRedo::UrlUndoRedo(QObject *parent)
    : QObject(parent)
{
}

void UrlUndoRedo::add(const QString &url)
{
    // Only remove the item after the m_currentUrlIndex
    if(m_currentUrlIndex < (m_urls.count() - 1)) {
        while((m_urls.count() - 1) > m_currentUrlIndex)
        {
            m_urls.removeLast();
        }
    }

    // Append new item
    if(!url.isEmpty()) {
        m_urls.append(url);
        m_currentUrlIndex = m_urls.count() - 1;
        emit currentUrlChanged();
    }
}

void UrlUndoRedo::next()
{
    if((m_currentUrlIndex + 1) < m_urls.count()) {
        ++m_currentUrlIndex;
        emit currentUrlChanged();
    }
}

void UrlUndoRedo::previous()
{
    if((m_currentUrlIndex - 1) < m_urls.count() && (m_currentUrlIndex - 1) >= 0) {
        --m_currentUrlIndex;
        emit currentUrlChanged();
    }
}

QString UrlUndoRedo::currentUrl()
{
    return m_urls.at(m_currentUrlIndex);
}

bool UrlUndoRedo::hasNext()
{
    if(m_currentUrlIndex < m_urls.count()) {
        return true;
    }
    return false;
}

bool UrlUndoRedo::hasPrevious()
{
    if(m_currentUrlIndex > 0) {
        return true;
    }
    return false;
}
