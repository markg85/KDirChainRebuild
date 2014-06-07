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
    m_lister = &m_listModel->m_lister;
    connect(m_lister, &KDirListerV2::directoryContentChanged, this, &DirGroupedModel::slotDirectoryContentChanged);
    connect(m_lister, &KDirListerV2::completed, this, &DirGroupedModel::slotCompleted);
    connect(this, &DirGroupedModel::groupbyChanged, this, &DirGroupedModel::regroup);
    connect(m_listModel, &DirListModel::pathChanged, [&](){ emit pathChanged(); });
}

DirGroupedModel::~DirGroupedModel()
{

}

void DirGroupedModel::setPath(const QString &path)
{
    qDebug() << "New path in C++ side:" << path << m_listModel->path();
    if(path != m_listModel->path()) {

        // Clean current administrative values.
        beginRemoveRows(QModelIndex(), 0, m_currentRowCount);
        clearAdministrativeData();
        endRemoveRows();

        // if we where already listing this folder then we need to jumpstart this model.
        if(m_lister->isListing(path)) {
            slotDirectoryContentChanged(m_lister->directory(path));
        }

        // Now set the new path. From there on the slotDirectoryContentChanged should take over if more details flow in.
        m_listModel->setPath(path);
    }
}

const QString &DirGroupedModel::path()
{
    return m_listModel->path();
}

void DirGroupedModel::setDetails(const QString &details)
{
    if(m_listModel->details() != details) {
        m_listModel->setDetails(details);
        emit detailsChanged();
    }
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
    return m_currentRowCount;
}

QVariant DirGroupedModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        qDebug() << index;
        return QVariant();
    } else {
        if((index.column() + Qt::UserRole + 1) == GroupedName) {
            return m_distinctGroupKey.at(index.row());
        }
    }

    return QVariant();
}

void DirGroupedModel::slotDirectoryContentChanged(KDirectory *dir)
{
    connect(dir, &KDirectory::entryDetailsChanged, this, &DirGroupedModel::processEntry, Qt::UniqueConnection);

    // We don't need to figure out the groups if we're not using groups to begin with.
    if(m_groupby == DirListModel::None) {
        if(m_groupList.isEmpty()) {
            beginInsertRows(QModelIndex(), m_currentRowCount, m_currentRowCount);
            DirGroupedProxyModel* model = new DirGroupedProxyModel(this);
            model->setFilterRole(m_groupby);
            model->setSourceModel(m_listModel);
            m_groupList << model;
            m_distinctGroupKey << QVariant();
            m_currentRowCount = m_distinctGroupKey.count();
            endInsertRows();
        }
        return;
    }

    int currentEntryCount = dir->entries().count();
    for(int i = m_currentEntryRowCount; i < currentEntryCount; i++) {
        processEntry(dir, i);
    }

    m_currentEntryRowCount = currentEntryCount;
}

void DirGroupedModel::slotCompleted(KDirectory *dir)
{
    // If we have remaining entries in this last signal we need to process them.
    if(dir->count() > m_currentEntryRowCount) {
        slotDirectoryContentChanged(dir);
    }
}

void DirGroupedModel::clearAdministrativeData()
{
    m_distinctGroupKey.clear();
    m_groupList.clear();
    m_currentRowCount = 0;
    m_currentEntryRowCount = 0;
}

void DirGroupedModel::processEntry(KDirectory *dir, int id)
{
    QVariant potentialNewGroupKey;
    const KDirectoryEntry& e = dir->entry(id);
    switch (m_groupby) {
    case DirListModel::Name:
        potentialNewGroupKey = e.name();
        break;
    case DirListModel::BaseName:
        potentialNewGroupKey = e.basename();
        break;
    case DirListModel::Extension:
        potentialNewGroupKey = e.extension();
        break;
    case DirListModel::MimeComment:
        potentialNewGroupKey = e.mimeComment();
        break;
    case DirListModel::MimeIcon:
        potentialNewGroupKey = e.iconName();
        break;
    case DirListModel::Thumbnail:
        break;
    case DirListModel::Size:
        if(!e.detailsLoaded()) dir->loadEntryDetails(id);
        potentialNewGroupKey = e.size();
        break;
    case DirListModel::ModificationTime:
        if(!e.detailsLoaded()) dir->loadEntryDetails(id);
        potentialNewGroupKey = e.time(KDirectoryEntry::FileTimes::ModificationTime);
        break;
    case DirListModel::AccessTime:
        if(!e.detailsLoaded()) dir->loadEntryDetails(id);
        potentialNewGroupKey = e.time(KDirectoryEntry::FileTimes::AccessTime);
        break;
    case DirListModel::CreationTime:
        if(!e.detailsLoaded()) dir->loadEntryDetails(id);
        potentialNewGroupKey = e.time(KDirectoryEntry::FileTimes::CreationTime);
        break;
    case DirListModel::User:
        if(!e.detailsLoaded()) dir->loadEntryDetails(id);
        potentialNewGroupKey = e.user();
        break;
    case DirListModel::Group:
        if(!e.detailsLoaded()) dir->loadEntryDetails(id);
        potentialNewGroupKey = e.group();
        break;
    case DirListModel::Hidden:
    case DirListModel::None:
        return; // Hidden and none do nothing here. Return if we encounter them.
    }

    // Specially don't check for potentialNewGroupKey.isNull() because you might very group on something where empty would be valid.
    // For example, grouping on extension leaves out folders since they don't have an extension.
    if(!m_distinctGroupKey.contains(potentialNewGroupKey)) {
        qDebug() << "insert row." << m_currentRowCount << m_distinctGroupKey.count() << m_distinctGroupKey;
        beginInsertRows(QModelIndex(), m_currentRowCount, m_distinctGroupKey.count());
        DirGroupedProxyModel* model = new DirGroupedProxyModel(this);
        model->setFilterRole(m_groupby);
        model->setRoleValueMatch(potentialNewGroupKey);
        model->setSourceModel(m_listModel);
        m_groupList << model;
        m_distinctGroupKey << potentialNewGroupKey;
        m_currentRowCount = m_distinctGroupKey.count();
        endInsertRows();
    }
}

void DirGroupedModel::regroup()
{
    if(!m_listModel->m_dir) {
        return;
    }

    qDebug() << "Regroup called...";
    qDebug() << "Regrouping dir:" << m_listModel->path() << m_listModel->m_dir->url();

    beginRemoveRows(QModelIndex(), 0, m_currentRowCount);
    clearAdministrativeData();
    endRemoveRows();
    slotDirectoryContentChanged(m_listModel->m_dir);
}

DirGroupedProxyModel* DirGroupedModel::modelAtIndex(int index)
{
    if(index == 0 && m_groupby == DirListModel::Roles::None && m_groupList.isEmpty()) {
        DirGroupedProxyModel* model = new DirGroupedProxyModel(this);
        model->setFilterRole(m_groupby);
        model->setSourceModel(m_listModel);
        m_groupList << model;
    }

    if(index >= 0 && index < m_groupList.count()) {
        return m_groupList.at(index);
    }
    return 0;
}

void DirGroupedModel::reload()
{
    m_listModel->reload();
    regroup();
}

void DirGroupedModel::setInputFilter(const QString &input)
{
    for(DirGroupedProxyModel* l : m_groupList) {
        l->setInputFilter(input);
    }
}

QHash<int, QByteArray> DirGroupedModel::roleNames() const
{
    static const QHash<int, QByteArray> roleNames {
        {GroupedName,     "groupedName"},
    };

    return roleNames;
}

