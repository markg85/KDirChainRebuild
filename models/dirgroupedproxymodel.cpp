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

#include "dirgroupedproxymodel.h"
#include "dirgroupedmodel.h"

#include <QDebug>

DirGroupedProxyModel::DirGroupedProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_filterValue()
    , m_inputFilter()
    , m_hiddenFiles(true)
{
    connect(this, &QSortFilterProxyModel::layoutChanged, [&](){
        qDebug() << "Layout changed...";
    });
}

void DirGroupedProxyModel::setRoleValueMatch(QVariant value)
{
    if(m_filterValue != value) {
        m_filterValue = value;
    }
}

void DirGroupedProxyModel::sort(int column, Qt::SortOrder order)
{
    // We use the rolenames for sorting, translating back to column number for the actual sorting.
    int calculatedColumn = column - (Qt::UserRole + 1);
    QSortFilterProxyModel::sort(calculatedColumn, order);
}

void DirGroupedProxyModel::reload()
{
    DirGroupedModel* model = qobject_cast<DirGroupedModel *>(sourceModel());
    model->reload();
}

bool DirGroupedProxyModel::hiddenFilesVisible()
{
    return m_hiddenFiles;
}

void DirGroupedProxyModel::setHiddenFilesVisible(bool hiddenFiles)
{
    if(hiddenFiles != m_hiddenFiles) {
        m_hiddenFiles = hiddenFiles;
        emit hiddenChanged();
        invalidateFilter();
    }
}

void DirGroupedProxyModel::setInputFilter(const QString &input)
{
    if(m_inputFilter != input) {
        m_inputFilter = input;
        invalidateFilter();

        // Why do i need to reset it after an empty string is given?
        // If i don't reset then the QML side somehow "forgets" to draw some items despite the rowCount being as it should be..
        if(input == "") {
            beginResetModel();
            endResetModel();
        }
    }
}

bool DirGroupedProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(filterRole() == DirListModel::None) {
        return true;
    } else {
        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

        // If we don't want to show hidden files...
        if(!m_hiddenFiles) {
            QVariant val = sourceModel()->data(index, DirListModel::Hidden);
            if(val.toBool()) {
                return false;
            }
        }

        QVariant val = sourceModel()->data(index, filterRole());
        if(val == m_filterValue) {
            if(m_inputFilter.isEmpty()) {
                return true;
            } else {
                const QString& fileName = sourceModel()->data(index, DirListModel::Name).toString(); // filename + extension
                return fileName.contains(QRegExp(m_inputFilter, Qt::CaseInsensitive));
            }
        } else {
            return false;
        }
    }
}
