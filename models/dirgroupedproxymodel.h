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

#ifndef DIRGROUPEDPROXYMODEL_H
#define DIRGROUPEDPROXYMODEL_H

#include <QSortFilterProxyModel>
#include "dirlistmodel.h"

class DirGroupedProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool hiddenFilesVisible READ hiddenFilesVisible WRITE setHiddenFilesVisible NOTIFY hiddenChanged)

public:
    DirGroupedProxyModel(QObject *parent = 0);
    void setRoleValueMatch(QVariant value);
    Q_INVOKABLE void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    Q_INVOKABLE void reload();

    bool hiddenFilesVisible();
    void setHiddenFilesVisible(bool hiddenFiles);
    void setInputFilter(const QString& input);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

signals:
    void hiddenChanged();

private:
    QVariant m_filterValue;
    QString m_inputFilter; // This is what the user types to filter on.
    bool m_hiddenFiles;
};

#endif
