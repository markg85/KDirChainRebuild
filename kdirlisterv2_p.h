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

#ifndef KDIRLISTERV2_P_H
#define KDIRLISTERV2_P_H

// Qt includes
#include <QObject>
#include <QStringList>
#include <QHash>

// KDE includes
#include <KIO/Job>

#include "kdirlisterv2.h"
#include "kdirectory.h"

class KDirListerV2Private : public QObject
{
    Q_OBJECT
public:
    explicit KDirListerV2Private(KDirListerV2* dirLister);

    void addUrl(QString url, KDirListerV2::OpenUrlFlags flags);
    void addUrl(KDirListerV2::DirectoryFetchDetails dirFetchDetails);
    void newUrl(KDirListerV2::DirectoryFetchDetails dirFetchDetails);
    bool isListing(const QString& url);
    KDirectory* directory(const QString& url);
    
signals:
    void directoryContentChanged(KDirectory* directoryContent);
    void completed(KDirectory* directoryContent);

// Just for those values that don't need a function.. Remember, we are in a private class here anyway!
public:
    KDirListerV2* q;
    QHash<QString, KDirectory*> m_cache;
};

#endif // KDIRLISTERV2_P_H
