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


#include "flatdirgroupedsortmodel.h"
#include <QCollator>
#include <QElapsedTimer>
#include <QDebug>
#include <algorithm>
#include <iostream>
#include <valgrind/callgrind.h>


FlatDirGroupedSortModel::FlatDirGroupedSortModel(QObject *parent)
    : QAbstractProxyModel(parent)
    , m_listModel(new DirListModel(this))
    , m_groupby(DirListModel::None) // No grouping by default
    , m_collator()
    , m_fromProxyToSource()
    , m_fromSourceToProxy()
    , m_threadPool(2) // a thread pool with two threads waiting for your command.
{
    // This makes sure sorting is done in a natural way. Aka, 1, 2, 3, ... 9, 10 instead of 1, 10, ...
    m_collator.setNumericMode(true);
    m_collator.setCaseSensitivity(Qt::CaseInsensitive);

    setSourceModel(m_listModel);
    connect(m_listModel, &DirListModel::pathChanged, [&](){ emit pathChanged(); });
    connect(m_listModel, &DirListModel::detailsChanged, [&](){ emit detailsChanged(); });

    connect(sourceModel(), &QAbstractListModel::rowsInserted, this, &FlatDirGroupedSortModel::modelRowsInserted);
    connect(sourceModel(), &QAbstractListModel::rowsRemoved, this, &FlatDirGroupedSortModel::modelRowsRemoved);

    connect(sourceModel(), &QAbstractListModel::dataChanged, [&](const QModelIndex &topLeft, const QModelIndex &bottomRight){
        this->dataChanged(topLeft, bottomRight);
    });

    connect(sourceModel(), &QAbstractListModel::layoutAboutToBeChanged, [&](){
        this->layoutAboutToBeChanged();
    });

    connect(sourceModel(), &QAbstractListModel::layoutChanged, [&](){
        this->layoutChanged();
    });

    connect(sourceModel(), &QAbstractListModel::modelAboutToBeReset, [&](){
        // Removing all rows on a reset means that we can animate the removal. This model should never reset!
        this->modelRowsRemoved(QModelIndex(), 0, this->rowCount());
    });
}

void FlatDirGroupedSortModel::setPath(const QString &path)
{
    m_listModel->setPath(path);
}

void FlatDirGroupedSortModel::setDetails(const QString &details)
{
    m_listModel->setDetails(details);
}

void FlatDirGroupedSortModel::setGroupby(int role)
{
    DirListModel::Roles enumRole = static_cast<DirListModel::Roles>(role);
    if(m_groupby != enumRole) {
        m_groupby = enumRole;
        emit groupbyChanged();
        regroup();
    }
}

void FlatDirGroupedSortModel::sort(int column, Qt::SortOrder order)
{
    // Actual sort the list we just composed. This just prepares the proxy id's in the right order.
    if(order == Qt::AscendingOrder) {
        if(column == DirListModel::Name) {
            // Special case for DirListModel::Name since it's using a natural string compare.
            std::sort(m_fromProxyToSource.begin(), m_fromProxyToSource.end(), [&](int a, int b) {
                return m_collator.compare(m_listModel->data(a, DirListModel::Name).toString(), m_listModel->data(b, DirListModel::Name).toString()) < 0;
            });
        } else {
            std::sort(m_fromProxyToSource.begin(), m_fromProxyToSource.end(), [&](int a, int b) {
                return variantLessThan(m_listModel->data(a, column), m_listModel->data(b, column));
            });
        }
    } else {
        if(column == DirListModel::Name) {
            // Special case for DirListModel::Name since it's using a natural string compare.
            std::sort(m_fromProxyToSource.begin(), m_fromProxyToSource.end(), [&](int a, int b) {
                return m_collator.compare(m_listModel->data(b, DirListModel::Name).toString(), m_listModel->data(a, DirListModel::Name).toString()) < 0;
            });
        } else {
            std::sort(m_fromProxyToSource.begin(), m_fromProxyToSource.end(), [&](int a, int b) {
                return variantLessThan(m_listModel->data(b, column), m_listModel->data(a, column));
            });
        }
    }

    // Now update the bookkeeping vectors. This applies the new sort order, but still doesn't make it visible yet.
    for(const int i : m_fromProxyToSource) {
        m_sortedProxyIds[i] = true;
        m_fromSourceToProxy[m_fromProxyToSource[i]] = i;
    }

    // Emit data change signal for all rows.
    emit dataChanged(createIndex(0, 0), createIndex(m_fromProxyToSource.size(), 0));
}

void FlatDirGroupedSortModel::sortGroup(int column, const QString &groupValue, Qt::SortOrder order)
{
    std::cout << "Main thread id: " << std::this_thread::get_id() << std::endl;
    m_threadPool.enqueue(&FlatDirGroupedSortModel::sortGroup_Thread, this, column, groupValue, order);
}

void FlatDirGroupedSortModel::sortGroup_Thread(int column, const QString &groupValue, Qt::SortOrder order)
{
    std::cout << "Worker thread id: " << std::this_thread::get_id() << std::endl;
    QElapsedTimer t;
    t.restart();

    // First create (and fill) a new vector that only contains the indexes for the current group
    QVector<int> indexesInThisGroup;

    // We only need to know the proxy id's for updating m_fromSourceToProxy later on to tell the view which elements have changed.
    QVector<int> proxyIndexesInThisGroup;

    // We use m_fromProxyToSource instead of m_fromSourceToProxy for one reason. In the sort functions below we need the source indexes.
    // If we would have used m_fromSourceToProxy then we would have to translate those proxy id's back to source id's. Which is easy and
    // fast, but this is probably (not tested) faster because i leave out the additional translation.
    for(const int i : m_fromProxyToSource) {
        const QString& groupByValue = m_listModel->data(i, m_groupby).toString();
        if(groupByValue == groupValue) {
            indexesInThisGroup.append(i);
            proxyIndexesInThisGroup.append(m_fromSourceToProxy[i]);
        }
    }

    // We need to update the mapping, but we lose mapping order if we sort. So we copy the vector.
    // Then - once sorted - we can map the new id's back to the old id's and update the proxy <> source mapping.
    const QVector<int> oldIndexesInThisGroup = indexesInThisGroup;

    // Sort!
    if(order == Qt::AscendingOrder) {
        if(column == DirListModel::Name) {
            // Special case for DirListModel::Name since it's using a natural string compare.
            std::sort(indexesInThisGroup.begin(), indexesInThisGroup.end(), [&](int a, int b) {
                return m_nameCache.at(a) < m_nameCache.at(b);
                //return m_collator.compare(m_listModel->data(a, DirListModel::Name).toString(), m_listModel->data(b, DirListModel::Name).toString()) < 0;
            });
        } else {
            std::sort(indexesInThisGroup.begin(), indexesInThisGroup.end(), [&](int a, int b) {
                return variantLessThan(m_listModel->data(a, column), m_listModel->data(b, column));
            });
        }
    } else {
        if(column == DirListModel::Name) {
            // Special case for DirListModel::Name since it's using a natural string compare.
            std::sort(indexesInThisGroup.begin(), indexesInThisGroup.end(), [&](int a, int b) {
                return m_nameCache.at(b) < m_nameCache.at(a);
                //return m_collator.compare(m_listModel->data(b, DirListModel::Name).toString(), m_listModel->data(a, DirListModel::Name).toString()) < 0;
            });
        } else {
            std::sort(indexesInThisGroup.begin(), indexesInThisGroup.end(), [&](int a, int b) {
                return variantLessThan(m_listModel->data(b, column), m_listModel->data(a, column));
            });
        }
    }

    // Update proxy -> source mapping
    const int numOfItems = indexesInThisGroup.size();
    for(int i = 0; i < numOfItems; i++) {
        m_fromProxyToSource[m_fromSourceToProxy[oldIndexesInThisGroup[i]]] = indexesInThisGroup[i];
    }

    // Update source -> proxy mapping
    for(const int i : proxyIndexesInThisGroup) {
        m_fromSourceToProxy[m_fromProxyToSource[i]] = i;
    }

    // Notify the view about the changed rows.
    const auto result = std::minmax_element(proxyIndexesInThisGroup.begin(), proxyIndexesInThisGroup.end());
    emit dataChanged(createIndex(*result.first, 0), createIndex(*result.second, 0));
    qDebug() << QString("Done sorting. It took: %1 ms").arg(t.elapsed());
}

QModelIndex FlatDirGroupedSortModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column);
}

QModelIndex FlatDirGroupedSortModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int FlatDirGroupedSortModel::rowCount(const QModelIndex &) const
{
    return m_listModel->rowCount();
}

int FlatDirGroupedSortModel::columnCount(const QModelIndex &parent) const
{
    return m_listModel->columnCount(parent);
}

QModelIndex FlatDirGroupedSortModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if(sourceIndex.isValid()) {
        const int proxyRow = m_fromProxyToSource.at(sourceIndex.row());
        return sourceModel()->index(proxyRow, sourceIndex.column(), sourceIndex.parent());
    }
    return QModelIndex();
}

QModelIndex FlatDirGroupedSortModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if(proxyIndex.isValid()) {
        const int sourceRow = m_fromProxyToSource.at(proxyIndex.row());
        return sourceModel()->index(sourceRow, proxyIndex.column(), proxyIndex.parent());
    }
    return QModelIndex();
}

void FlatDirGroupedSortModel::modelRowsInserted(const QModelIndex & parent, int start, int end)
{
    beginInsertRows(parent, start, end);


    // We first simply add the new entries to our bookkeeping vectors. Ordering them will happen later.
    for(int i = start; i <= end; i++) {
        m_fromProxyToSource.append(i);
        m_fromSourceToProxy.append(i);
        m_nameCache.append(m_collator.sortKey(m_listModel->data(i, DirListModel::Name).toString()));
    }

    // As soon as we insert new rows, we remove the cache to know which items we have sorted.
    m_sortedProxyIds.reserve(m_fromProxyToSource.size());
    m_sortedProxyIds.fill(false);

    if(m_groupby != DirListModel::None) {
        orderNewEntries(start, end);
    }

    endInsertRows();
}

void FlatDirGroupedSortModel::modelRowsRemoved(const QModelIndex & parent, int start, int end)
{
    beginRemoveRows(parent, start, end);

    // This is OBVIOUSLY wrong! It should be removing those that should actually be removed...
    // This will do as a temporary workaround :)
    m_fromProxyToSource.clear();
    m_fromSourceToProxy.clear();
    m_itemsPerGroup.clear();
    m_nameCache.clear();
    m_sortedProxyIds.fill(false);

    endRemoveRows();
}

void FlatDirGroupedSortModel::orderNewEntries(int start, int end)
{
    // Create a temporary vector containing our new indexes.
    QVector<int> newEntries;
    for(int i = start; i <= end; i++) {
        newEntries.append(i);
    }

    // Sort based on grouping key
    std::sort(newEntries.begin(), newEntries.end(), [&](int a, int b) {
        return m_listModel->data(a, m_groupby).toString().compare(m_listModel->data(b, m_groupby).toString()) < 0;
    });

    // Update our bookkeeping vectors
    const int newSize = newEntries.size();
    for(int i = 0; i < newSize; i++) {
        // New proxy to source index becomes:
        m_fromProxyToSource[i + start] = newEntries[i];

        // New source to proxy index becomes:
        m_fromSourceToProxy[newEntries[i]] = i + start;

        const QString& groupVal = m_listModel->data(i + start, m_groupby).toString();
        if(m_itemsPerGroup.contains(groupVal)) {
            const int curCount = m_itemsPerGroup.value(groupVal) + 1;
            m_itemsPerGroup.insert(groupVal, curCount);
        } else {
            m_itemsPerGroup.insert(groupVal, 1);
        }
    }
}

void FlatDirGroupedSortModel::regroup()
{
    // Clean the current grouping counts
    m_itemsPerGroup.clear();

    // And do the actual regrouping
    orderNewEntries(0, this->rowCount() - 1);

    for(const int i : m_fromSourceToProxy) {
        qDebug() << i << m_listModel->data(i, DirListModel::Name).toString();
    }

    // Notify the model of the new changed data.
    // For later: optimize here. Only emit dataChange for those rows in the current view.
    emit dataChanged(createIndex(0, 0), createIndex(this->rowCount() - 1, 0));
}

void FlatDirGroupedSortModel::reload()
{
    m_listModel->reload();
}

void FlatDirGroupedSortModel::requestSortForItems(int startId, int endId, bool isMovingDown)
{
    // Add some more items to sort. Just so the we fill up a bit of a sorted cache and prevent calling this function too often.
    int modifiedEndId = endId;
    if((modifiedEndId + 100) < this->rowCount()) {
        modifiedEndId += 100;
    } else {
        modifiedEndId = this->rowCount();
    }

    // Return early. If both the start and end id are known in our sorted cache then everything in between is sorted
    if(m_sortedProxyIds.at(startId) && m_sortedProxyIds.at(endId)) {
//        qDebug() << "Return early. Everything in between is sorted already.";
        return;
    }

    int modifiedStartId = endId;

    // We only need this temporary to fix out mapping back from source to proxy
    bool needToRunNthElement = false;

    if(startId > 0 && m_sortedProxyIds.at(startId - 1)) {
        // now loop through the id's from start to end and check which ones we've already sorted to shorten our list of elements to sort
        for(int i = startId; i <= modifiedEndId; i++) {
            if(!m_sortedProxyIds.at(i)) {
                modifiedStartId = i;
                // We stop this loop as soon as we found an id that isn't in our list of items known to be sorted
                break;
            }
        }
    } else {
        // This true is only half true. It should be false for startId == 0, but it's easier to check that down below.
        needToRunNthElement = true;
        modifiedStartId = startId;
    }

    // We could have no items to sort. Just because it's all sorted already.
    if(modifiedStartId == modifiedEndId) {
        return;
    }

    CALLGRIND_START_INSTRUMENTATION;

    // Is the entry before our current entry sorted?
    QTime t;
    if(needToRunNthElement && startId > 0) {
        qDebug() << "running std::nth_element";
        t.restart();
        /*
         * nth_element guarantees that the item at position m_fromProxyToSource.begin() + modifiedStartId should be at that position
         * as if it where sorted. All items before m_fromProxyToSource.begin() + modifiedStartId are also guaranteed to be before it
         * although not in a sorted order.
         */
        std::nth_element(m_fromProxyToSource.begin(), m_fromProxyToSource.begin() + modifiedStartId, m_fromProxyToSource.end(), [&](int a, int b) {
//            return m_collator.compare(m_listModel->data(a, DirListModel::Name).toString(), m_listModel->data(b, DirListModel::Name).toString()) < 0;
            return m_nameCache.at(a) < m_nameCache.at(b);
        });

        qDebug() << "nth element took:" << QString("%1 ms").arg(QString::number(t.elapsed()));
    }

    t.restart();
    qDebug() << "running std::partial_sort. Start =" << modifiedStartId << "end =" << modifiedEndId;
    const int numOfItems = modifiedEndId - modifiedStartId;
    std::partial_sort(m_fromProxyToSource.begin() + modifiedStartId, m_fromProxyToSource.begin() + modifiedStartId + numOfItems, m_fromProxyToSource.end(), [&](int a, int b) {
//        return m_collator.compare(m_listModel->data(a, DirListModel::Name).toString(), m_listModel->data(b, DirListModel::Name).toString()) < 0;
        return m_nameCache.at(a) < m_nameCache.at(b);
    });
    qDebug() << "partial sort took:" << QString("%1 ms").arg(QString::number(t.elapsed()));

    CALLGRIND_STOP_INSTRUMENTATION;


    // Now update the bookkeeping vectors. This applies the new sort order, but still doesn't make it visible yet.
    for(int i = modifiedStartId; i < modifiedStartId + numOfItems; i++) {
        m_sortedProxyIds[i] = true;
        m_fromSourceToProxy[m_fromProxyToSource[i]] = i;
    }

    // Emit data change signal for all rows that "might" have been changed due to this sort operation.
    // A view will pick this event up and update the visual. Here the user sees the re-sorting.
    emit dataChanged(createIndex(modifiedStartId, 0), createIndex(modifiedEndId, 0));
}

int FlatDirGroupedSortModel::numOfItemsForGroup(const QString &group)
{
    return m_itemsPerGroup.value(group);
}

QString FlatDirGroupedSortModel::stringRole(int role)
{
//    DirListModel::Roles enumRole = static_cast<DirListModel::Roles>(role);
    return roleNames().value(role);
}

bool FlatDirGroupedSortModel::variantLessThan(const QVariant &l, const QVariant &r)
{
    switch (l.userType()) {
    case QVariant::Invalid:
        return (r.type() != QVariant::Invalid);
    case QVariant::Int:
        return l.toInt() < r.toInt();
    case QVariant::UInt:
        return l.toUInt() < r.toUInt();
    case QVariant::LongLong:
        return l.toLongLong() < r.toLongLong();
    case QVariant::ULongLong:
        return l.toULongLong() < r.toULongLong();
    case QMetaType::Float:
        return l.toFloat() < r.toFloat();
    case QVariant::Double:
        return l.toDouble() < r.toDouble();
    case QVariant::Char:
        return l.toChar() < r.toChar();
    case QVariant::Date:
        return l.toDate() < r.toDate();
    case QVariant::Time:
        return l.toTime() < r.toTime();
    case QVariant::DateTime:
        return l.toDateTime() < r.toDateTime();
    case QVariant::String:
    default:
        return l.toString().compare(r.toString()) < 0;
    }
    return false;
}
