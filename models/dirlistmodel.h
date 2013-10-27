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

#ifndef DIRLISTMODEL_H
#define DIRLISTMODEL_H

#include <QAbstractListModel>
#include "kdirlisterv2.h"
#include "kdirectory.h"

class DirListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        Name = Qt::UserRole + 1,
        BaseName,
        Extension,
        MimeComment,
        MimeIcon,
        Thumbnail,
        Size,
        ModificationTime,
        AccessTime,
        CreationTime,
        User,
        Group
    };

    /**
     * @param parent parent qobject
     */
    explicit DirListModel(QObject* parent = 0);
    ~DirListModel();

    void setPath(const QString& path);
    const QString& path();

    /// Reimplemented from QAbstractItemModel.
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    /// Reimplemented from QAbstractItemModel.
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual QHash<int, QByteArray> roleNames() const;
    virtual QVariant headerName(int role) const;

    void slotDirectoryContentChanged(KDirectory* dir);
    void slotCompleted(KDirectory* dir);

private:
    KDirListerV2* m_lister;
    KDirectory* m_dir;
    QString m_path;
    int m_currentRowCount;
    bool m_doneLoading;
};


#endif
