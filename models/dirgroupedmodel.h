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

#ifndef DIRGROUPEDMODEL_H
#define DIRGROUPEDMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVector>
#include <QVariant>
#include "dirlistmodel.h"
#include "dirgroupedproxymodel.h"
#include "kdirlisterv2.h"
#include "kdirectory.h"

class DirGroupedModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QString details READ details WRITE setDetails NOTIFY detailsChanged)
    Q_PROPERTY(int groupby READ groupby WRITE setGroupby NOTIFY groupbyChanged)
    Q_ENUMS(Roles)

public:
    enum Roles {
        GroupedName = Qt::UserRole + 1,
    };

    /**
     * @param parent parent qobject
     */
    explicit DirGroupedModel(QObject* parent = 0);
    ~DirGroupedModel();

    void setPath(const QString& path);
    const QString& path();

    void setDetails(const QString& details);
    const QString& details() { return m_listModel->details(); }

    DirListModel::Roles groupby();
    void setGroupby(int role);

    /// Reimplemented from QAbstractItemModel.
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    void slotDirectoryContentChanged(KDirectory* dir);
    void slotCompleted(KDirectory* dir);
    void clearAdministrativeData();

    void processEntry(KDirectory *dir, int id);
    Q_INVOKABLE void regroup();

    Q_INVOKABLE DirGroupedProxyModel* modelAtIndex(int index);
    Q_INVOKABLE void reload();

    // This is what the user types when pressing CTRL + I.
    Q_INVOKABLE void setInputFilter(const QString& input);

protected:
    virtual QHash<int, QByteArray> roleNames() const;

signals:
    void pathChanged();
    void detailsChanged();
    void groupbyChanged();

private:
    DirListModel* m_listModel;
    KDirListerV2* m_lister;
    DirListModel::Roles m_groupby;
    QVector<QVariant> m_distinctGroupKey; // The key you group in - mime for example - can and likely will occur multiple times. This vector just stores the same keys but without duplicates.
    QList<DirGroupedProxyModel*> m_groupList; // This stores a DirListModel per grouped component. This is what views will use to display a "group".
    int m_currentRowCount;
    int m_currentEntryRowCount;
};

#endif
