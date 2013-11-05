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

#include "dirgroupedmodel.h"
#include "kdirectoryentry.h"

#include <QDebug>

DirGroupedModel::DirGroupedModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_listModel(0)
    , m_lister(0)
    , m_groupby()
    , m_distinctGroupKey()
    , m_groupList()
    , m_currentRowCount(0)
    , m_currentEntryRowCount(0)
{
    m_listModel = new DirListModel(this);
    m_lister = m_listModel->m_lister;
    connect(m_lister, &KDirListerV2::directoryContentChanged, this, &DirGroupedModel::slotDirectoryContentChanged);
    connect(m_lister, &KDirListerV2::completed, this, &DirGroupedModel::slotCompleted);
    connect(this, &DirGroupedModel::groupbyChanged, this, &DirGroupedModel::regroup);
    connect(m_listModel, &DirListModel::pathChanged, [&](){ emit pathChanged();});
}

DirGroupedModel::~DirGroupedModel()
{

}

void DirGroupedModel::setPath(const QString &path)
{
    m_listModel->setPath(path);
}

const QString &DirGroupedModel::path()
{
    return m_listModel->path();
}

DirListModel::Roles DirGroupedModel::groupby()
{
    return m_groupby;
}

void DirGroupedModel::setGroupby(int role)
{
    DirListModel::Roles enumRole = static_cast<DirListModel::Roles>(role);
    if(enumRole != m_groupby) {
        m_groupby = enumRole;
        emit groupbyChanged();
    }
}

int DirGroupedModel::rowCount(const QModelIndex &) const
{
    // If we supplied DirListModel::Roles::None then m_listModel is going to be returned as the "grouped" model. Jsut without grouping.
    if(m_groupby == DirListModel::Roles::None) {
        return 1;
    } else {
        return m_currentRowCount;
    }
}

QVariant DirGroupedModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        qDebug() << index;
        return QVariant();
    } else {
        if(role == Qt::DisplayRole && index.row() >= 0 && index.row() < m_distinctGroupKey.count()) {
            return m_distinctGroupKey.at(index.row());
        }
    }

    return QVariant();
}

void DirGroupedModel::slotDirectoryContentChanged(KDirectory *dir)
{
    QVector<QVariant> newGroupKeys;
    int currentEntryCount = dir->entries().count();
    for(int i = m_currentEntryRowCount; i < currentEntryCount; i++) {
        const KDirectoryEntry& e = dir->entry(i);
        switch (m_groupby) {
        case DirListModel::Name:
            break;
        case DirListModel::BaseName:
            break;
        case DirListModel::Extension:
            break;
        case DirListModel::MimeComment:
            break;
        case DirListModel::MimeIcon:
            if(!m_distinctGroupKey.contains(e.iconName()) && !newGroupKeys.contains(e.iconName())) {
                newGroupKeys << e.iconName();
            }
            break;
        case DirListModel::Thumbnail:
            break;
        case DirListModel::Size:
            break;
        case DirListModel::ModificationTime:
            break;
        case DirListModel::AccessTime:
            break;
        case DirListModel::CreationTime:
            break;
        case DirListModel::User:
            break;
        case DirListModel::Group:
            break;
        }
    }

    if(newGroupKeys.count() > 0) {
        int newRows = newGroupKeys.count() + m_distinctGroupKey.count() - 1;
        beginInsertRows(QModelIndex(), 0, newRows);
        foreach (const QVariant& val, newGroupKeys) {
            DirGroupedProxyModel* model = new DirGroupedProxyModel(this);
            model->setRoleFilter(m_groupby, val);
            model->setSourceModel(m_listModel);
            m_groupList << model;
            m_distinctGroupKey << val;
        }
        m_currentRowCount = m_distinctGroupKey.count();
        endInsertRows();
    }
    m_currentEntryRowCount = currentEntryCount;

    qDebug() << "Row count: " << m_currentRowCount;
}

void DirGroupedModel::slotCompleted(KDirectory *dir)
{
    // If we have remaining entries in this last signal we need to process them.
    if(dir->count() > m_currentEntryRowCount) {
        slotDirectoryContentChanged(dir);
    }
}

void DirGroupedModel::regroup()
{
    if(!m_listModel->m_dir) {
        return;
    }

    m_distinctGroupKey.clear();
    m_groupList.clear();
    m_currentEntryRowCount = 0;
    slotDirectoryContentChanged(m_listModel->m_dir);
}

DirGroupedProxyModel* DirGroupedModel::modelAtIndex(int index)
{
    if(index >= 0 && index < m_groupList.count()) {
        return m_groupList.at(index);
    }
    return 0;
}

