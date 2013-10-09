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

#ifndef KDIRECTORY_H
#define KDIRECTORY_H

#include <QObject>
#include <QDir>
#include <KIO/Job>

#include "kdirectoryentry.h"

class KDirectoryPrivate;

class KDirectory : public QObject
{
    Q_OBJECT
public:
    explicit KDirectory(const QString& directory, QObject *parent = 0);
    
    virtual const QVector<KDirectoryEntry>& entries();
    virtual const KDirectoryEntry& entry(int index);
    virtual const QString& url();
    virtual int count();
    virtual void setDetails(const QString& details);

    QDir::Filters filter();
    void setFilter(QDir::Filters filters);
    QDir::SortFlags sorting();
    void setSorting(QDir::SortFlags sort);

signals:
    void entriesProcessed(KDirectory* dir);
    void completed(KDirectory* dir);

private:
    KDirectoryPrivate *const d;
};

Q_DECLARE_TYPEINFO(KDirectory, Q_MOVABLE_TYPE);

#endif // KDIRECTORY_H
