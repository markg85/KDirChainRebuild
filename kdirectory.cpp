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

#include "kdirectory.h"
#include "kdirectoryprivate_p.h"




KDirectory::KDirectory(const QString& directory, QObject *parent)
    : QObject(parent)
    , d(new KDirectoryPrivate(this, directory))
{
    // I sadly have to catch the signals and re-emit them with the current KDirectory object. I don't know of a better way (yet).
    connect(d, SIGNAL(entriesProcessed()), this, SLOT(entriesProcessed()));
    connect(d, SIGNAL(completed()), this, SLOT(completed()));
}

const QList<KDirectoryEntry> &KDirectory::entries()
{
    return d->m_dirEntries;
}

const QList<KDirectoryEntry> &KDirectory::entryInfoList(QDir::Filters filters, QDir::SortFlags sort)
{
    return d->entryInfoList(filters, sort);
}

const KDirectoryEntry &KDirectory::entryLookup(int index)
{
    return d->entryLookup(index);
}

const QString &KDirectory::url()
{
    return d->m_directory;
}

int KDirectory::count()
{
    return d->count();
}

void KDirectory::setDetails(const QString &details)
{
    d->m_details = details;
}

QDir::Filters KDirectory::filter()
{
    return d->filter();
}

void KDirectory::setFilter(QDir::Filters filters)
{
    d->setFilter(filters);
}

QDir::SortFlags KDirectory::sorting()
{
    return d->sorting();
}

void KDirectory::setSorting(QDir::SortFlags sort)
{
    d->setSorting(sort);
}

void KDirectory::entriesProcessed()
{
    emit entriesProcessed(this);
}

void KDirectory::completed()
{
    emit completed(this);
}
