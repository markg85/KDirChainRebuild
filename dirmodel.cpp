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

// The only reason i need this class is because i can't call the .parent()
// function on a QModelIndex from within the ::parent() function.
// I need to know the "parent" row.
class ParentHelper
{
public:
    ParentHelper(int r, int c)
        : row(r)
        , column(c)
    {}

    int row;
    int column;
    ParentHelper *parent;
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
    QModelIndex returnIndex;
    if(parent.isValid()) {
        returnIndex = createIndex(row, column, parent.internalPointer());
    } else if(row >= 0) {
        returnIndex = createIndex(row, column, new ParentHelper(0, 0));
    } else {
        returnIndex = QModelIndex();
//        return createIndex(row, column, new ParentHelper(0, 0));
    }
//    qDebug() << "DirModel::index row:" << row << "column:" << column << "returnIndex:" << returnIndex;
    return returnIndex;
}

QModelIndex DirModel::parent(const QModelIndex &index) const
{
    QModelIndex mdlIndex;
    if(index.isValid()) {

//        qDebug() << "--> DirModel::parent 000";
        // Get our ParentHelper, get the row/column and return it as a new index.
        ParentHelper* idx = static_cast<ParentHelper*>(index.internalPointer());
        if(idx) {
            mdlIndex = createIndex(idx->row, idx->column);
        }
    }
//    qDebug() << "DirModel::parent return index:" << mdlIndex << "index:" << index;

    return index;
}

int DirModel::rowCount(const QModelIndex &parent) const
{
    KDirectory* dir;

    // When there is no valid parent, try to fetch the root item.
    if(!parent.isValid()) {
        dir = d->m_lister->directory(0);
    } else {
        // Try to get the Directory from the current row.
        dir = d->m_lister->directory(parent.row());
    }

    dir->setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
    dir->setSorting(QDir::DirsFirst);

    // Check dir, if it exists, return whatever count it has. Otherwise 0.
    qDebug() << "DirModel::rowCount parent:" << parent;
    if(!dir) {
        return 0;
    } else {
        return dir->count();
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

bool DirModel::hasChildren(const QModelIndex &parent) const
{
//    qDebug() << "DirModel::hasChildren parent:" << parent;

    // If we are invalid we are at the root level and always have some children.
    if(!parent.isValid()) {
        return true;
    } else {
        // Lookup the actual item. If it is a folder, set this to true. Otherwise false.
        ParentHelper* idx = static_cast<ParentHelper*>(parent.internalPointer());
        if(!idx) {
            return false;
        } else {
            KDirectory* dir = d->m_lister->directory(idx->row);
            if(!dir) {
                return false;
            }
            if(parent.row() < dir->count()) {
                return dir->entryLookup(parent.row()).isDir();
            }
        }
        return false;
    }
}

bool DirModel::canFetchMore(const QModelIndex &parent) const
{
//    qDebug() << "DirModel::canFetchMore parent:" << parent;

    if(parent.isValid()) {
        // This newDir... prevents calling "fetchMore" twice.
        if(d->m_newDirJustRequested) {
            d->m_newDirJustRequested = false;
            return false;
        }
        d->m_newDirJustRequested = true;

        return true;
    } else {
        return false;
    }
}

void DirModel::fetchMore(const QModelIndex &parent)
{
//    qDebug() << "DirModel::fetchMore parent:" << parent << "Name:" << data(parent, Qt::DisplayRole);
//    qDebug() << "DirModel::fetchMore parents parent:" << parent.parent();


    QModelIndex parentTraversal = parent;
    QString pathTillRoot;

    do {

//        qDebug() << "parent traversal..";

        QVariant currentName = data(parent, Qt::DisplayRole);
        QString stringName("/");
        if(currentName.canConvert(QVariant::String)) {
            stringName.prepend(currentName.toString());
            pathTillRoot.prepend(stringName);
        }
        parentTraversal = parent.parent();

    } while(parentTraversal.isValid() && parent != parentTraversal);

    ParentHelper* idx = static_cast<ParentHelper*>(parentTraversal.internalPointer());
    if(!idx) {
        qFatal("(ParentHelper) Failed to do parent lookup.");
    }

    KDirectory* rootDir = d->m_lister->directory(idx->row);
    if(!rootDir) {
        qFatal("(KDirectory) Failed to do parent lookup.");
    }

    QString newUrl = rootDir->url() + pathTillRoot;
    d->m_lister->openUrl(newUrl);

    qDebug() << "--> root dir:" << rootDir->url();
    qDebug() << "--> pathTillRoot:" << pathTillRoot;
}

void DirModel::reload()
{
    d->m_lister->openUrl(d->m_url, KDirListerV2::Reload);
}

QVariant DirModel::data(const QModelIndex &index, int role) const
{
//    qDebug() << "DirModel::data index:" << index;

    // We need to have a valid index. No valid index is no data.
    if(!index.isValid()) {
        return QVariant();
    }

    // Next we need to know the parent index. This needs to be know so that we can fetch the actual folder data.
    ParentHelper* idx = static_cast<ParentHelper*>(index.internalPointer());
    if(!idx) {
        return QVariant();
    }

    // Fetch the actual directory object. This contains all data.
    KDirectory* dir = d->m_lister->directory(idx->row);
    if(!dir || dir->count() <= index.row()) {
        return QVariant();
    }

    // Now we need to have the actual file data. If the directory is valid, this will very likely be valid as well.
    KDirectoryEntry entry = dir->entryLookup(index.row());

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
    , m_newDirJustRequested(false)
{
    connect(m_lister, SIGNAL(directoryContentChanged(KDirectory*)), this, SLOT(itemsAdded(KDirectory*)));
    connect(m_lister, SIGNAL(completed(KDirectory*)), this, SLOT(folderCompleted(KDirectory*)));
    connect(m_lister, SIGNAL(clear()), this, SLOT(slotClear()));
}


void DirModelPrivate::itemsAdded(KDirectory *dir)
{
//    qDebug() << "DirModelPrivate::itemsAdded";
    if(dir->count() > 0) {

//        qDebug() << "DirModelPrivate::itemsAdded IF";

        q->beginInsertRows(QModelIndex(), 0, dir->count());
        q->endInsertRows();

//        q->layoutChanged();
        emit q->countChanged();
    }
}

void DirModelPrivate::folderCompleted(KDirectory *dir)
{
    Q_UNUSED(dir)
//    qDebug() << "DirModelPrivate::folderCompleted";
}

void DirModelPrivate::slotClear()
{
//    qDebug() << "DirModelPrivate::slotClear";
    q->beginResetModel();
    q->endResetModel();
}
