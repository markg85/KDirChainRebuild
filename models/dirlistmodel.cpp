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

#include "dirlistmodel.h"
#include <QModelIndex>
#include <QDebug>


DirListModel::DirListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_lister(0)
    , m_dir()
    , m_path()
    , m_currentRowCount(0)
    , m_doneLoading(false)
{
    m_lister = new KDirListerV2();

    connect(m_lister, &KDirListerV2::directoryContentChanged, this, &DirListModel::slotDirectoryContentChanged);
    connect(m_lister, &KDirListerV2::completed, this, &DirListModel::slotCompleted);
}

DirListModel::~DirListModel()
{
    // Delete pointers...
}

void DirListModel::setPath(const QString &path)
{
    if(m_path != path) {
        m_path = path;
    }

    if(!m_lister->isListing(m_path)) {
        m_lister->openUrl(m_path);
    }
}

const QString &DirListModel::path()
{
    return m_path;
}

QVariant DirListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        qDebug() << index;
        return QVariant();
    } else {

        KDirectoryEntry entry = m_dir->entry(index.row());

        //qDebug() << index.row() << entry.name();

        switch (role) {
        case Qt::DisplayRole:
            return QVariant(entry.name());
            break;
        }
    }

    return QVariant();
}

int DirListModel::rowCount(const QModelIndex &parent) const
{
    if(m_dir) {
        return m_currentRowCount;
    }
    return 0;
}

QHash<int, QByteArray> DirListModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[FileName] = "fileName";
    roleNames[BaseName] = "baseName";
    roleNames[Extension] = "extension";
    return roleNames;
}

void DirListModel::slotDirectoryContentChanged(KDirectory *dir)
{
    if(!m_dir && dir) {
        m_dir = dir;
    }

    beginInsertRows(QModelIndex(), m_currentRowCount, m_dir->count() - 1);
    m_currentRowCount = m_dir->count();
    endInsertRows();

    //qDebug() << "Count: " << m_dir->count();
}

void DirListModel::slotCompleted(KDirectory *dir)
{
    slotDirectoryContentChanged(dir);
    m_doneLoading = true;
}
