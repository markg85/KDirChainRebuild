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
{
    connect(this, &BreadcrumbUrlModel::urlChanged, &BreadcrumbUrlModel::parseUrl);
}

BreadcrumbUrlModel::~BreadcrumbUrlModel()
{

}

void BreadcrumbUrlModel::setUrl(const QString &url)
{
    if(m_url != QUrl(url)) {
        m_url = url;
        emit urlChanged();
    }
}

const QString BreadcrumbUrlModel::url()
{
    m_url.setPath(m_stringList.join(QDir::separator()));
    return m_url.toDisplayString();
}

void BreadcrumbUrlModel::append(QString str)
{
    if(!str.isEmpty()) {
        beginInsertRows(QModelIndex(), m_stringList.count(), m_stringList.count());
        m_stringList << str;
        m_url.setPath(m_stringList.join(QDir::separator()));
        endInsertRows();
    }
}

void BreadcrumbUrlModel::parent()
{
    if(m_stringList.count() > 0) {
        beginRemoveRows(QModelIndex(), m_stringList.count() - 1, m_stringList.count());
        m_stringList.removeLast();
        m_url.setPath(m_stringList.join(QDir::separator()));
        endRemoveRows();
    }
}

void BreadcrumbUrlModel::removeAfterIndex(int index)
{
    index += 1; // Remember, remove everything _after_ the given index.
    if(index >= 0 && index <= m_stringList.count()) {
        beginRemoveRows(QModelIndex(), index, m_stringList.count());
        for(int i = m_stringList.count(); i > index; i--) {
            m_stringList.removeLast();
        }
        m_url.setPath(m_stringList.join(QDir::separator()));
        endRemoveRows();
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
    m_stringList = m_url.path().split(QDir::separator(), QString::SkipEmptyParts);
    emit layoutChanged();
}
