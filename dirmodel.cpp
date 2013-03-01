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

#include "dirmodel.h"

#include <QVariant>
#include <KDirLister>
#include <QDebug>

#include <klocale.h>

#include "kdirectoryentry.h"
#include "kdirlisterv2.h"

class ParentSomething
{
public:
    ParentSomething(int r, int c)
        : row(r)
        , column(c)
    {}

    int row;
    int column;
    ParentSomething *parent;
};


DirModel::DirModel(QObject *parent)
    : QAbstractItemModel(parent)
    , d(new DirModelPrivate(this))

{
//    QThread* workerThread = new QThread(this);
//    d->moveToThread(workerThread);
//    workerThread->start();

    QHash<int, QByteArray>roleNames;
    roleNames[Qt::DecorationRole]      = "decoration";
    roleNames[DirModel::Name]          = "Name";
//    roleNames[DirModel::Size]          = "Size";
    roleNames[DirModel::ModifiedTime]  = "ModifiedTime";
    roleNames[DirModel::Permissions]   = "Permissions";
    roleNames[DirModel::Owner]         = "Owner";
    roleNames[DirModel::Group]         = "Group";
    roleNames[DirModel::Type]          = "Type"; // Type == item.mimeComment() ... yeah, go figure that one out.
    roleNames[DirModel::ColumnCount]   = "ColumnCount";
    roleNames[DirModel::UrlRole]       = "Url";
    roleNames[DirModel::MimeTypeRole]  = "MimeType";
    roleNames[DirModel::IconName]      = "IconName";
    roleNames[DirModel::BaseName]      = "BaseName";
    roleNames[DirModel::Extension]     = "Extension";
    roleNames[DirModel::TimeString]    = "TimeString";
    roleNames[DirModel::MimeOrThumb]   = "MimeOrThumb";
    setRoleNames(roleNames);
}

DirModel::~DirModel()
{
}

int DirModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

void DirModel::openUrl(const QString& url)
{
    d->m_lister->openUrl(url);
    d->m_url = url;
    emit urlChanged();
}

const QString &DirModel::url()
{
    return d->m_url;
}

QModelIndex DirModel::index(int row, int column, const QModelIndex &parent) const
{

//    qDebug() << "row:" << row << "column:" << column << "parent:" << parent << "isValid:" << parent.isValid();
//    qDebug() << "mmm:" << createIndex(row, column) << parent;

    if(row >= 0 && column >= 0) {
        // A valid row + column, but no valid parent. So the parent is root.
        if(!parent.isValid()) {
            return createIndex(row, column, new ParentSomething(0, 0));
        } else {
            return createIndex(row, column, new ParentSomething(parent.row(), parent.column()));
        }
    } else {
        return QModelIndex();
    }
}

QModelIndex DirModel::parent(const QModelIndex &index) const
{
    ParentSomething* idx = static_cast<ParentSomething*>(index.internalPointer());
    if(idx) {

        qDebug() << "ppp index:" << index << "row:" << idx->row << "col:" << idx->column;
        return createIndex(idx->row, idx->column);
    }
    return QModelIndex();
}

int DirModel::rowCount(const QModelIndex &parent) const
{
    qDebug() << "DirModel::rowCount:" << parent;
//    if(!parent.isValid()) {
//        return 0;
//    }

    ParentSomething* idx = static_cast<ParentSomething*>(parent.internalPointer());
    if(!idx) {
        // No parent at all yet.. Try to get the root item.
        idx = new ParentSomething(0, 0);
    }

    qDebug() << "DirModel::rowCount fetching directory with index (row):" << idx->row;

    KDirectory* currentDir = d->m_lister->directory(idx->row);

    if(currentDir) {
        return currentDir->count();
    } else {
        return 0;
    }
}

QVariant DirModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        switch (role) {
        case Qt::DisplayRole:
            switch (section) {
            case Name:
                return i18nc("@title:column","Name");
            case Size:
                return i18nc("@title:column","Size");
            case ModifiedTime:
                return i18nc("@title:column","Date");
            case Permissions:
                return i18nc("@title:column","Permissions");
            case Owner:
                return i18nc("@title:column","Owner");
            case Group:
                return i18nc("@title:column","Group");
            case Type:
                return i18nc("@title:column","Type");
            case UrlRole:
                return i18nc("@title:column","UrlRole");
            case MimeTypeRole:
                return i18nc("@title:column","MimeTypeRole");
            case IconName:
                return i18nc("@title:column","IconName");
            case BaseName:
                return i18nc("@title:column","BaseName");
            case Extension:
                return i18nc("@title:column","Extension");
            case TimeString:
                return i18nc("@title:column","TimeString");
            case MimeOrThumb:
                return i18nc("@title:column","MimeOrThumb");
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags DirModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return 0;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool DirModel::hasChildren(const QModelIndex &parent) const
{
    qDebug() << "DirModel::hasChildren" << parent;
    if(!parent.isValid()) {
        return true;
    }
//    return true;
}

void DirModel::reload()
{
    d->m_lister->openUrl(d->m_url, KDirListerV2::Reload);
}

QVariant DirModel::data(const QModelIndex &index, int role) const
{
    // We need to have a valid index. No valid index is no data.
    if(!index.isValid()) {
        return QVariant();
    }

    // Next we need to know the parent index. This needs to be know so that we can fetch the actual folder data.
    ParentSomething* idx = static_cast<ParentSomething*>(index.internalPointer());
    if(!idx) {
        return QVariant();
    }

    // Fetch the actual directory object. This contains all data.
    KDirectory* dir = d->m_lister->directory(idx->row);
    if(!dir || dir->count() <= index.row()) {
        return QVariant();
    }

    // Now we need to have the actual file data. If the directory is valid, this will very likely be valid as well.
    KDirectoryEntry entry = dir->entries().at(index.row());

    switch(role) {
    case Qt::DisplayRole: {
        switch(index.column()) {
//        switch(role) {
        case Name:
            return entry.name();
        case Size:
            return 0;
        case ModifiedTime:
            return 0;
        case Permissions:
            return 0;
        case Owner:
            return 0;
        case Group:
            return 0;
        case Type:
            return 0;
        case UrlRole:
            return 0;
        case MimeTypeRole:
            return 0;
        case IconName:
            return entry.iconName();
        case BaseName:
            return entry.basename();
        case Extension:
            return entry.extension();
        case TimeString:
            return 0;
        case MimeOrThumb:
            return "image://mime/" + entry.iconName();
        }
    }
    }

    return QVariant();
}


DirModelPrivate::DirModelPrivate(DirModel *model)
    : q(model)
    , m_lister(new KDirListerV2())
    , m_url()
{
    connect(m_lister, SIGNAL(directoryContentChanged(KDirectory*)), this, SLOT(itemsAdded(KDirectory*)));
    connect(m_lister, SIGNAL(completed(KDirectory*)), this, SLOT(folderCompleted(KDirectory*)));
    connect(m_lister, SIGNAL(clear()), this, SLOT(slotClear()));
}


void DirModelPrivate::itemsAdded(KDirectory *dir)
{
    qDebug() << "DirModelPrivate::itemsAdded";
    if(dir->count() > 0) {

        qDebug() << "DirModelPrivate::itemsAdded IF";

//        q->beginInsertRows(QModelIndex(), 0, dir->count());
//        q->endInsertRows();

        q->layoutChanged();
        emit q->countChanged();
    }
}

void DirModelPrivate::folderCompleted(KDirectory *dir)
{
    qDebug() << "Rowcount:" << q->rowCount();
}

void DirModelPrivate::slotClear()
{
//    q->beginResetModel();
//    q->endResetModel();
}
