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

#include "kdirlisterv2_p.h"

// Qt includes
#include <QDebug>


KDirListerV2Private::KDirListerV2Private(KDirListerV2* dirLister)
    : q(dirLister)
    , m_urlToIndex()
    , m_lruCache(10)
{
}

void KDirListerV2Private::addUrl(QString url, KDirListerV2::OpenUrlFlags flags)
{
    KDirListerV2::DirectoryFetchDetails dirFetchDetails;
    dirFetchDetails.url = url;
    dirFetchDetails.openFlags = flags;

    addUrl(dirFetchDetails);
}

void KDirListerV2Private::addUrl(KDirListerV2::DirectoryFetchDetails dirFetchDetails)
{
    // We take a different path if we want to reload a url that is currently being monitored.
    // Otherwise we add a new url
    if(m_lruCache.get(dirFetchDetails.url.toStdString()) && dirFetchDetails.openFlags.testFlag(KDirListerV2::Reload)) {
        // Remove URL from m_lruCache.
        m_lruCache.remove(dirFetchDetails.url.toStdString());
    }

    qDebug() << "Added new url:" << dirFetchDetails.url;
    newUrl(dirFetchDetails);
}

void KDirListerV2Private::newUrl(KDirListerV2::DirectoryFetchDetails dirFetchDetails)
{
    KDirectory* dir = new KDirectory(dirFetchDetails.url);
    dir->setSorting(dirFetchDetails.sorting);
    dir->setFilter(dirFetchDetails.filters);
    dir->setDetails(dirFetchDetails.details);

    // Add node to list. This list will stay and will only get shorter (dir removed) if the physical directory is removed
    // Or if some cache mechanism kicks in that decided this dir is useless weight.
    m_lruCache.set(dirFetchDetails.url.toStdString(), dir);

    // And we make some connections
    connect(dir, SIGNAL(entriesProcessed(KDirectory*)), this, SIGNAL(directoryContentChanged(KDirectory*)));
    connect(dir, SIGNAL(completed(KDirectory*)), this, SIGNAL(completed(KDirectory*)));
    connect(dir, SIGNAL(completed(KDirectory*)), this, SLOT(printLRUStats()));
}

void KDirListerV2Private::printLRUStats()
{
    qDebug() << "--------------- LRU STATISTICS ---------------";
    qDebug() << "Count: " << m_lruCache.count();
    qDebug() << "stat_cache_hits: " << m_lruCache.stat_cache_hits;
    qDebug() << "stat_cache_miss: " << m_lruCache.stat_cache_miss;
    qDebug() << "stat_cache_get: " << m_lruCache.stat_cache_get;
    qDebug() << "stat_cache_set: " << m_lruCache.stat_cache_set;
    qDebug() << "stat_cache_out: " << m_lruCache.stat_cache_out;
    qDebug() << "--------------- LRU STATISTICS ---------------";
}
