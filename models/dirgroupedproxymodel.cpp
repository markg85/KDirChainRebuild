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

#include <QDebug>

DirGroupedProxyModel::DirGroupedProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_acceptedRole()
    , m_filterValue()
{

}

void DirGroupedProxyModel::setRoleFilter(DirListModel::Roles acceptedRole, QVariant value)
{
    if(m_acceptedRole != acceptedRole) {
        m_acceptedRole = acceptedRole;
    }
    m_filterValue = value;
}

bool DirGroupedProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(m_acceptedRole == DirListModel::None) {
        return true;
    } else {
        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
        QVariant val = sourceModel()->data(index, m_acceptedRole);
        if(val == m_filterValue) {
            return true;
        } else {
            return false;
        }
    }
}
