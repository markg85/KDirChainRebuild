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

#include "breadcrumburlmodel.h"

#include <QDir>
#include <QDebug>

BreadcrumbUrlModel::BreadcrumbUrlModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_url()
    , m_stringList()
    , m_urls()
    , m_currentUrlIndex(0)
{
}

BreadcrumbUrlModel::~BreadcrumbUrlModel()
{

}

void BreadcrumbUrlModel::setUrl(const QString &url)
{
    setUrl(QUrl(url));
}

void BreadcrumbUrlModel::setUrl(const QUrl &url)
{
    while(m_urls.count() > m_currentUrlIndex + 1) {
        m_urls.removeLast();
    }

    if(m_url != url) {
        m_urls.append(url.toString());
        m_currentUrlIndex = m_urls.count() - 1;
        parseUrl();
    }
}

const QString BreadcrumbUrlModel::url()
{
    return m_url.toString();
}

void BreadcrumbUrlModel::append(QString str)
{
    if(!str.isEmpty()) {
        m_stringList << str;
        QUrl newUrl(m_url);
        newUrl.setPath(QDir::separator() + m_stringList.join(QDir::separator()));
        setUrl(newUrl);
    }
}

void BreadcrumbUrlModel::parent()
{
    if(m_stringList.count() > 0) {
        m_stringList.removeLast();
        QUrl newUrl(m_url);
        newUrl.setPath(QDir::separator() + m_stringList.join(QDir::separator()));
        setUrl(newUrl);
    }
}

void BreadcrumbUrlModel::removeAfterIndex(int index)
{
    index += 1; // Remember, remove everything _after_ the given index.
    if(index >= 0 && index < m_stringList.count()) {
        for(int i = m_stringList.count(); i > index; i--) {
            m_stringList.removeLast();
        }

        QUrl newUrl(m_url);
        newUrl.setPath(QDir::separator() + m_stringList.join(QDir::separator()));
        setUrl(newUrl);
    }
}

QString BreadcrumbUrlModel::protocol()
{
    return m_url.scheme();
}

QString BreadcrumbUrlModel::username()
{
    return m_url.userName();
}

QString BreadcrumbUrlModel::password()
{
    return m_url.password();
}

int BreadcrumbUrlModel::port()
{
    return m_url.port();
}

void BreadcrumbUrlModel::next()
{
    if(hasNext()) {
        m_currentUrlIndex += 1;
        parseUrl();
    }
}

void BreadcrumbUrlModel::previous()
{
    if(hasPrevious()) {
        m_currentUrlIndex -= 1;
        parseUrl();
    }
}

bool BreadcrumbUrlModel::hasNext()
{
    // +1 because count is human countable (starting from 1) and index is machine countable (starting from 0)
    if(m_urls.count() > (m_currentUrlIndex + 1)) {
        return true;
    }
    return false;
}

bool BreadcrumbUrlModel::hasPrevious()
{
    if(m_currentUrlIndex > 0 && m_currentUrlIndex < m_urls.count()) {
        return true;
    }
    return false;
}

int BreadcrumbUrlModel::rowCount(const QModelIndex &parent) const
{
    return m_stringList.count();
}

QVariant BreadcrumbUrlModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    } else {
        if(role == Qt::DisplayRole && index.row() < m_stringList.count()) {
            return m_stringList.at(index.row());
        }
    }

    return QVariant();
}

void BreadcrumbUrlModel::parseUrl()
{
    m_url = m_urls.at(m_currentUrlIndex);
    m_stringList = m_url.path().split(QDir::separator(), QString::SkipEmptyParts);
    emit urlChanged();
    emit layoutChanged();
}
