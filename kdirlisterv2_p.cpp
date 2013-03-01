/* This file is part of the KDE project
   Copyright (C) 2013 Mark Gaiser <markg85@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kdirlisterv2_p.h"

// Qt includes
#include <QDebug>

// KDE includes
#include <KUrl>
#include <kde_file.h>


KDirListerV2Private::KDirListerV2Private(KDirListerV2* dirLister)
    : q(dirLister)
    , m_dirs()
    , m_urlToIndex()
    , m_details("0")

{
    /*
    qDebug() << "KDirWatch method:" << m_watch->internalMethod();
    connect(KDirWatch::self(), SIGNAL(dirty(QString)), this, SLOT(slotDirty(QString)));
    connect(KDirWatch::self(), SIGNAL(created(QString)), this, SLOT(slotCreated(QString)));
    connect(KDirWatch::self(), SIGNAL(deleted(QString)), this, SLOT(slotDeleted(QString)));
    */
}

void KDirListerV2Private::addUrl(QString url, KDirListerV2::OpenUrlFlags flags)
{
    // We take a different path if we want to reload a url that is currently being monitored.
    // Otherwise we add a new url
    if(m_urlToIndex.contains(url) && flags.testFlag(KDirListerV2::Reload)) {
        removeUrlBookkeepingAndData(url);
    }

    newUrl(url);
}

void KDirListerV2Private::newUrl(QString url)
{
    // Sadly one KUrl..
    KUrl goodUrl(url);

    // Create the directory node object (stored later on in a list)
    DirectoryNode node(goodUrl.url());

    // Next we request a entry list from KIO for this URL.
    // Thought: this "could" move to DirectoryNode as well? If yes, i probably need to move the "details" from below to that class as well.
    node.m_job = KIO::listDir(goodUrl, KIO::HideProgressInfo);

    // If any details are set, pass them along to the listener.
    if(!m_details.isEmpty()) {
        node.m_job->addMetaData("details", m_details);
    }

    // Add node to list. This list will stay and will only get shorter (dir removed) if the physical directory is removed
    // Or if some cache mechanism kicks in that decided this dir is useless weight.
    m_dirs.append(node);
    int index = m_dirs.count() - 1;

    // The only additional bookkeeping we have (from url -> index. the list itself is the index -> node which contains the url as well)
    m_urlToIndex.insert(goodUrl.url(), index);

    // And we make some connections
    connect(node.m_job, SIGNAL(entries(KIO::Job*,KIO::UDSEntryList)), node.m_dir, SLOT(slotEntries(KIO::Job*,KIO::UDSEntryList)));
    connect(node.m_job, SIGNAL(result(KJob*)), node.m_dir, SLOT(slotResult(KJob*)));
    connect(node.m_dir, SIGNAL(entriesProcessed(KDirectory*)), this, SIGNAL(directoryContentChanged(KDirectory*)));
    connect(node.m_dir, SIGNAL(completed(KDirectory*)), this, SIGNAL(completed(KDirectory*)));
}

void KDirListerV2Private::removeUrlBookkeepingAndData(QString url)
{
    int index = urlToIndex(url);
    if(index < -1) {
        qFatal("Requested a non existing index to be removed!");
    }

    m_dirs.removeAt(index);
    m_urlToIndex.remove(url);
}

const QString KDirListerV2Private::indexToUrl(int index)
{
    // Assumption: if the index fits in the list, the object must be there and be valid. Empty is fine, but NOT 0 AKA segfault.
    if(index < m_dirs.size()) {
        return m_dirs.at(index).m_dir->url();
    } else {
        return QString();
    }
}

// Return -100 because QModelIndex is -1 when invalid.. And this will likely be used together with Qt models.
int KDirListerV2Private::urlToIndex(const QString &url)
{
    if(m_urlToIndex.contains(url)) {
        return m_urlToIndex.value(url);
    }
    return -100;
}

bool KDirListerV2Private::indexExists(const int index)
{
    if(index < m_dirs.size() && index >= 0) {
        return true;
    } else {
        return false;
    }
}

KDirectory *KDirListerV2Private::directory(const int index)
{
    // Assumption: if the index fits in the list, the object must be there and be valid. Empty is fine, but NOT 0 AKA segfault.
    if(index < m_dirs.size()) {
        return m_dirs.at(index).m_dir;
    } else {
        return 0;
    }
}
