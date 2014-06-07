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
#include "kdirlisterv2.h"
#include <QModelIndex>
#include <QDebug>


DirListModel::DirListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_lister()
    , m_dir()
    , m_emptyVariant()
    , m_path()
    , m_details("0")
    , m_currentRowCount(0)
    , m_roleCount(0)
    , m_doneLoading(false)
{
    m_roleCount = roleNames().count(); // This initializes the roleNames hash and fills the m_roleCount.

    connect(&m_lister, &KDirListerV2::directoryContentChanged, this, &DirListModel::slotDirectoryContentChanged);
    connect(&m_lister, &KDirListerV2::completed, this, &DirListModel::slotCompleted);
}

DirListModel::~DirListModel()
{
    // Delete pointers...
}

void DirListModel::setPath(const QString &path, bool reload)
{
    if(m_path != path) {
        m_path = path;
        beginResetModel();
//        beginRemoveRows(QModelIndex(), 0, m_currentRowCount);
        m_currentRowCount = 0;
//        endRemoveRows();
        endResetModel();
        emit pathChanged();
    }

    if(reload || !m_lister.isListing(m_path)) {
        KDirListerV2::DirectoryFetchDetails dirFetchDetails;
        dirFetchDetails.url = m_path;
        dirFetchDetails.details = m_details;
        dirFetchDetails.filters = QDir::NoDotAndDotDot;

        if(reload) {
            dirFetchDetails.openFlags = KDirListerV2::Reload;
        }

        m_lister.openUrl(dirFetchDetails);
    } else {
        slotCompleted(m_lister.directory(m_path));
    }
}

const QString &DirListModel::path()
{
    return m_path;
}

void DirListModel::setDetails(const QString &details)
{
    if(m_details != details) {
        m_details = details;
        emit detailsChanged();
    }
}

QVariant DirListModel::data(const QModelIndex &index, int role) const
{
//    qDebug() << "Data call. Role:" << role << "row:" << index.row();

    if (!index.isValid()) {
        qDebug() << index;
        return QVariant();
    } else {

        int switchVal;
        if(role > Qt::UserRole) {
            switchVal = role;
        } else {
            switchVal = index.column() + Qt::UserRole + 1;
        }

        if(role == Qt::DisplayRole || role > Qt::UserRole) {
            return data(index.row(), switchVal);


        } else if (role == Qt::DecorationRole && index.column() == 0) {
            // display the file icon. This is to be implemented.
        }
    }

    return m_emptyVariant;
}

QVariant DirListModel::data(int index, int role) const
{
    const KDirectoryEntry& entry = m_dir->entry(index);

    switch (role) {
    case Name:
        return QVariant(entry.name());
        break;
    case BaseName:
        return QVariant(entry.basename());
        break;
    case Extension:
        return QVariant(entry.extension());
        break;
    case Hidden:
        return QVariant(entry.isHidden());
        break;
    case MimeComment:
        return QVariant(entry.mimeComment());
        break;
    case MimeIcon:
        return QVariant(entry.iconName());
        break;
    case Thumbnail:
        // Should return a thumbnail of the file.
        return QVariant("TO_BE_IMPLEMENTED");
        break;
    case Size:
        if(!entry.detailsLoaded()) m_dir->loadEntryDetails(index);
        return QVariant(entry.size());
        break;
    case ModificationTime:
        if(!entry.detailsLoaded()) m_dir->loadEntryDetails(index);
        return QVariant(entry.time(KDirectoryEntry::ModificationTime));
        break;
    case AccessTime:
        if(!entry.detailsLoaded()) m_dir->loadEntryDetails(index);
        return QVariant(entry.time(KDirectoryEntry::AccessTime));
        break;
    case CreationTime:
        if(!entry.detailsLoaded()) m_dir->loadEntryDetails(index);
        return QVariant(entry.time(KDirectoryEntry::CreationTime));
        break;
    case User:
        if(!entry.detailsLoaded()) m_dir->loadEntryDetails(index);
        return QVariant(entry.user());
        break;
    case Group:
        if(!entry.detailsLoaded()) m_dir->loadEntryDetails(index);
        return QVariant(entry.group());
        break;
    default:
        return QVariant();
    }
}

int DirListModel::rowCount(const QModelIndex &) const
{
    if(m_dir) {
        return m_currentRowCount;
    }
    return 0;
}

int DirListModel::columnCount(const QModelIndex &) const
{
    return m_roleCount;
}

QVariant DirListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        switch (role) {
        case Qt::DisplayRole:
            // Looks odd, but we want to use the role names as they are defined in the header.
            return headerName(Qt::UserRole + 1 + section);
        }
    }
    return QVariant();
}

QHash<int, QByteArray> DirListModel::roleNames() const
{
    static const QHash<int, QByteArray> roleNames {
        {Name,              "name"},
        {BaseName,          "baseName"},
        {Extension,         "extension"},
        {MimeComment,       "mimeComment"},
        {MimeIcon,          "mimeIcon"},
        {Thumbnail,         "thumbnail"},
        {Size,              "size"},
        {ModificationTime,  "modificationTime"},
        {AccessTime,        "accessTime"},
        {CreationTime,      "creationTime"},
        {User,              "user"},
        {Group,             "group"},
    };

    return roleNames;
}

QVariant DirListModel::headerName(int role) const
{
    // NOTE! this _MUST_ be in the same order as the roleNames!
    static const QVector<QByteArray> headerNames {
        "Name",
        "Base name",
        "Extension",
        "MIME Comment",
        "MIME Icon",
        "Thumbnail",
        "Size",
        "Modification time",
        "Access time",
        "Creation time",
        "User",
        "Group",
    };

    // We want to use the roles as defined in the header.
    const int newRole = role - (Qt::UserRole + 1);

    if(newRole < headerNames.count() && newRole >= 0) {
        return headerNames.at(newRole);
    } else {
        return "UNKNOWN_HEADER_NAME";
    }
}

void DirListModel::reload()
{
    beginRemoveRows(QModelIndex(), 0, m_currentRowCount);
    m_currentRowCount = 0;
    endRemoveRows();
    setPath(m_path, true);
}

void DirListModel::slotDirectoryContentChanged(KDirectory *dir)
{
    if((!m_dir && dir) || dir != m_dir) {
        m_dir = dir;
        connect(m_dir, &KDirectory::entryDetailsChanged, [&](KDirectory*, int id){
            // notify the view that the entry with "id" has changed data.
            QModelIndex topLeft = createIndex(id, 0);
            QModelIndex bottomRight = createIndex(id, m_roleCount - 1); // WHY -1? I have to do this if i hook it in a proxy. Why, i don't know.
            emit dataChanged(topLeft, bottomRight);
        });
    }

    if(m_currentRowCount < m_dir->count()) {
        qDebug() << "Inserting rows. begin =" << m_currentRowCount << "end =" << m_dir->count() - 1;
        beginInsertRows(QModelIndex(), m_currentRowCount, m_dir->count() - 1);
        m_currentRowCount = m_dir->count();
        endInsertRows();
    }
}

void DirListModel::slotCompleted(KDirectory *dir)
{
    // If we have remaining entries in this last signal we need to process them.
    if(dir->count() > m_currentRowCount) {
        slotDirectoryContentChanged(dir);
    }
    m_doneLoading = true;
}
