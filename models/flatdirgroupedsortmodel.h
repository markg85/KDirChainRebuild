/*
    Copyright (C) 2014 Mark Gaiser <markg85@gmail.com>

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

#ifndef FLATDIRGROUPEDSORTMODEL_H
#define FLATDIRGROUPEDSORTMODEL_H

#include <QObject>
#include <QVector>
#include <QCollator>
#include <QAbstractProxyModel>
#include "dirlistmodel.h"

#include "ThreadPool.h"

class FlatDirGroupedSortModel : public QAbstractProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QString details READ details WRITE setDetails NOTIFY detailsChanged)
    Q_PROPERTY(int groupby READ groupby WRITE setGroupby NOTIFY groupbyChanged)

public:
    FlatDirGroupedSortModel(QObject *parent = 0);

    void setPath(const QString& path);
    const QString& path() { return m_listModel->path(); }

    void setDetails(const QString& details);
    const QString& details() { return m_listModel->details(); }

    void setGroupby(int role);
    DirListModel::Roles groupby() { return m_groupby; }

    Q_INVOKABLE void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    Q_INVOKABLE void sortGroup(int column, const QString& groupValue, Qt::SortOrder order = Qt::AscendingOrder);
    void sortGroup_Thread(int column, const QString& groupValue, Qt::SortOrder order = Qt::AscendingOrder);

    virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex & index) const;

    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;

    virtual QModelIndex mapFromSource(const QModelIndex & sourceIndex) const;
    virtual QModelIndex mapToSource(const QModelIndex & proxyIndex) const;

    void modelRowsInserted(const QModelIndex &, int, int);
    void modelRowsRemoved(const QModelIndex &, int, int);

    void orderNewEntries(int start, int end);
    void regroup();

    Q_INVOKABLE void reload();
    Q_INVOKABLE void requestSortForItems(int startId, int endId, bool isMovingDown);
    Q_INVOKABLE int numOfItemsForGroup(const QString& group);
    Q_INVOKABLE QString stringRole(int role);

    inline bool variantLessThan(const QVariant& l, const QVariant& r);

signals:
    void pathChanged();
    void detailsChanged();
    void groupbyChanged();

private:
    DirListModel* m_listModel;
    DirListModel::Roles m_groupby;
    QCollator m_collator;

    // Our bookkeeping vectors.
    QVector<int> m_fromProxyToSource;
    QVector<int> m_fromSourceToProxy;
    QVector<bool> m_sortedProxyIds;
    QList<QCollatorSortKey> m_nameCache;

    QHash<QString, int> m_itemsPerGroup;

    ThreadPool m_threadPool;
};


#endif
