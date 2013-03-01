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

#ifndef KDIRLISTERV2_P_H
#define KDIRLISTERV2_P_H

// Qt includes
#include <QObject>
#include <QStringList>
#include <QHash>

// KDE includes
#include <KDirWatch>
#include <KIO/Job>

#include "kdirlisterv2.h"
#include "kdirectory.h"

class DirectoryNode
{
public:
    DirectoryNode(const QString& url)
        : m_job(0)
        , m_watch(KDirWatch::self())
        , m_dir(new KDirectory(url))
    {
        m_watch->addDir(url);
    }

    // Store the ListJob object (mainly for signals/slots)
    KIO::ListJob * m_job;

    // This calls KDirWatch::self() so it is the same pointer in every DirectoryNode instance.
    KDirWatch* m_watch;

    // Contains the actual directory details (url, count, files/folders...)
    KDirectory* m_dir;
};

class KDirListerV2Private : public QObject
{
    Q_OBJECT
public:
    explicit KDirListerV2Private(KDirListerV2* dirLister);

    void addUrl(QString url, KDirListerV2::OpenUrlFlags flags);
    void newUrl(QString url);
    void removeUrlBookkeepingAndData(QString url);

    // This function is a convenient function for models.
    const QString indexToUrl(int index);
    int urlToIndex(const QString& url);
    bool indexExists(const int index);
    KDirectory* directory(const int index);

    
signals:
    void directoryContentChanged(KDirectory* directoryContent);
    void completed(KDirectory* directoryContent);

public slots:
    /*
    void slotDirty(const QString& entry);
    void slotCreated(const QString& entry);
    void slotDeleted(const QString& entry);
    */

private:
    KDirListerV2* q;
    QList<DirectoryNode> m_dirs;
    QHash<QString, int> m_urlToIndex;


// Just for those values that don't need a function.. Remember, we are in a private class here anyway!
public:
    QString m_details;
};

#endif // KDIRLISTERV2_P_H
