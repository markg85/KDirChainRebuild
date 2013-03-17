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

#ifndef DIRMODEL_H
#define DIRMODEL_H

#include <QAbstractItemModel>
#include <QThread>
#include <QVariant>



#include "kdirlisterv2.h"
#include "kdirectory.h"


class DirModel;

/**
 * @brief The private class for DirModel. This class contains all logic.
 */
class DirModelPrivate : public QObject
{
    Q_OBJECT

public slots:
    void itemsAdded(KDirectory* dir);
    void folderCompleted(KDirectory* dir);
    void slotClear();

public:
    DirModelPrivate(DirModel* model);

    DirModel* q;
    KDirListerV2* m_lister;
    QString m_url;
    bool m_newDirJustRequested;
};

/**
 * This class is a rewritten KDirModel class. It's made in an attempt to make it faster and to get all stuff - as far as possible - out of the gui thread.
 *
 * @author Mark Gaiser <markg85@gmail.com>
 */
class DirModel : public QAbstractItemModel
{
    Q_OBJECT

    /**
     * @property string The url we want to browse. it may be an absolute path or a correct url of any protocol KIO supports
     */
    Q_PROPERTY(QString url READ url WRITE openUrl NOTIFY urlChanged)

    /**
     * @property count Total number of rows
     */
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles {
        Name = 0,
        Size,
        ModifiedTime,
        Permissions,
        Owner,
        Group,
        Type,
        UrlRole,
        MimeTypeRole,
        IconName,
        BaseName,
        Extension,
        TimeString,
        MimeOrThumb,
        ColumnCount
    };

    DirModel(QObject* parent=0);
    virtual ~DirModel();
    virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;

    void openUrl(const QString& url);
    const QString& url();

    QVariant data(const QModelIndex &index, int role) const;
    int count() const {return rowCount();}

    /// Reimplemented from QAbstractItemModel. O(1)
    virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;

    /// Reimplemented from QAbstractItemModel.
    virtual QModelIndex parent ( const QModelIndex & index ) const;

    /// Reimplemented from QAbstractItemModel.
    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

    /// Reimplemented from QAbstractItemModel. Returns the column titles.
    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    virtual bool hasChildren(const QModelIndex &parent) const;

    virtual bool canFetchMore(const QModelIndex &parent) const;

    virtual void fetchMore(const QModelIndex &parent);

    Q_INVOKABLE void reload();

signals:
    void countChanged();
    void urlChanged();

private:
    friend class DirModelPrivate;
    DirModelPrivate *const d;
};

#endif // DIRMODEL_H
