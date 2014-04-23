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
#include <QDebug>
#include <algorithm>
#include <iostream>


FlatDirGroupedSortModel::FlatDirGroupedSortModel(QObject *parent)
    : QAbstractProxyModel(parent)
    , m_listModel(new DirListModel(this))
    , m_groupby(DirListModel::None) // No grouping by default
    , m_collator()
    , m_fromProxyToSource()
    , m_fromSourceToProxy()
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
    }
}

void FlatDirGroupedSortModel::sort(int column, const QString &groupValue, Qt::SortOrder order)
{
    // First: get all proxy row id's for those rows where the value of m_groupBy = groupValue.
    const int allRows = m_fromProxyToSource.size();
    QVector<int> filteredRows;
    QHash<int, QVariant> filteredRowsDataCache;
    for(int i = 0; i < allRows; i++) {
        // Now get the row value for m_groupBy
        const QModelIndex idx = createIndex(m_fromProxyToSource[i], 0);
        const QString& groupByValue = sourceModel()->data(idx, m_groupby).toString();
        if(groupByValue == groupValue) {
            filteredRows.append(i);
            filteredRowsDataCache.insert(i, sourceModel()->data(idx, column));
        }
    }

    // We make a copy of the current filteredRows because we need to know the order it has _now_ after it has been sorted.
    const QVector<int> origFilteredRows = filteredRows;

    // Actual sort the list we just composed. This just prepares the proxy id's in the right order.
    // After thisstep we still need to update the actual positions in m_fromProxyToSource and m_fromSourceToProxy to apply the new order.
    if(order == Qt::AscendingOrder) {
        if(column == DirListModel::Name) {
            // Special case for DirListModel::Name since it's using a natural string compare.
//            std::partial_sort(filteredRows.begin(), filteredRows.begin() + 100, filteredRows.end(), [&](int a, int b) {
            std::sort(filteredRows.begin(), filteredRows.end(), [&](int a, int b) {
                return m_collator.compare(filteredRowsDataCache.value(a).toString(), filteredRowsDataCache.value(b).toString()) < 0;
            });
        } else {
            std::sort(filteredRows.begin(), filteredRows.end(), [&](int a, int b) {
                return variantLessThan(filteredRowsDataCache.value(a), filteredRowsDataCache.value(b));
            });
        }
    } else {
        if(column == DirListModel::Name) {
            // Special case for DirListModel::Name since it's using a natural string compare.
//            std::partial_sort(filteredRows.begin(), filteredRows.begin() + 100, filteredRows.end(), [&](int a, int b) {
            std::sort(filteredRows.begin(), filteredRows.end(), [&](int a, int b) {
                return m_collator.compare(filteredRowsDataCache.value(b).toString(), filteredRowsDataCache.value(a).toString()) < 0;
            });
        } else {
            std::sort(filteredRows.begin(), filteredRows.end(), [&](int a, int b) {
                return variantLessThan(filteredRowsDataCache.value(b), filteredRowsDataCache.value(a));
            });
        }
    }

    // A temporary vector to hold m_fromProxyToSource since we're going to swap values in m_fromProxyToSource.
    const QVector<int> tempPtS = m_fromProxyToSource;

    // Now update the bookkeeping vectors. This applies the new sort order, but still doesn't make it visible yet.
    const int numOfChangedItems = filteredRows.size();
    for(int i = 0; i < numOfChangedItems; i++) {

        const int origProxyKey = origFilteredRows[i];
        const int destProxyKey = filteredRows[i];

        const int origSourceKey = tempPtS[origProxyKey];
        const int destSourceKey = tempPtS[destProxyKey];

        // The actual swapping.. This keeps bookkeeping in order.
        m_fromProxyToSource[origProxyKey] = destSourceKey;
        m_fromSourceToProxy[origSourceKey] = destProxyKey;
    }

    // Nice C++11 :)
    // We could directly use origFilteredRows to get the first and last element?... Don't know if that will always work.
    const auto result = std::minmax_element(origFilteredRows.begin(), origFilteredRows.end());

    // Emit data change signal for all rows that "might" have been changed due to this sort operation.
    // A view will pick this event up and update the visual. Here the user sees the re-sorting.
    emit dataChanged(createIndex(*result.first, 0), createIndex(*result.second, 0));
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
        m_allSourceIndexes.append(i);

        // Should go..
        m_fromProxyToSource.append(i);
        m_fromSourceToProxy.append(i);
    }

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
    m_allSourceIndexes.clear();
    m_itemsPerGroup.clear();

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

void FlatDirGroupedSortModel::reload()
{
    m_listModel->reload();
}

void FlatDirGroupedSortModel::requestSortForItems(int startId, int endId)
{
    /* --------------------------------
     * This function is the beginning of very efficient sorting where it would only sort what you see
     * which makes it on demand. However, it still needs a LOT of work to work properly.
     * --------------------------------
     */
    int initStartId = startId;
    int initEndId = endId;

    if((startId - 50) > 0) {
        startId -= 50;
    } else {
        startId = 0;
    }

    if((endId + 50) < this->rowCount()) {
        endId += 50;
    } else {
        endId = this->rowCount();
    }

    QVector<int> tempPtS = m_allSourceIndexes;
    std::nth_element(tempPtS.begin(), tempPtS.begin() + startId, tempPtS.end(), [&](int a, int b) {
        return m_collator.compare(m_listModel->data(a, DirListModel::Name).toString(), m_listModel->data(b, DirListModel::Name).toString()) < 0;
    });

    const int numOfItems = endId - startId;
    std::partial_sort(tempPtS.begin() + startId, tempPtS.begin() + startId + numOfItems, tempPtS.end(), [&](int a, int b) {
        return m_collator.compare(m_listModel->data(a, DirListModel::Name).toString(), m_listModel->data(b, DirListModel::Name).toString()) < 0;
    });

    // Now update the bookkeeping vectors. This applies the new sort order, but still doesn't make it visible yet.
    for(int i = startId; i < startId + numOfItems; i++) {

        // Update proxy -> source and source -> proxy mapping
        if(m_fromProxyToSource[i] != tempPtS[i]) {
            m_fromProxyToSource[i] = tempPtS[i];
            m_fromSourceToProxy[tempPtS[i]] = i;
        }
    }

    // Emit data change signal for all rows that "might" have been changed due to this sort operation.
    // A view will pick this event up and update the visual. Here the user sees the re-sorting.
    emit dataChanged(createIndex(initStartId, 0), createIndex(initEndId, 0));
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
