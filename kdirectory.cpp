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

#include <QDebug>

KDirectory::KDirectory(const QString& directory, QObject *parent)
    : QObject(parent)
    , m_directory(directory)
    , m_dirEntries()
    , m_count(0)
{
}

const QList<KDirectoryEntry> &KDirectory::entries()
{
    return m_dirEntries;
}

const QString &KDirectory::url()
{
    return m_directory;
}

int KDirectory::count()
{
    return m_count;
}

void KDirectory::slotEntries(KIO::Job *job, const KIO::UDSEntryList &entries)
{
    if(entries.count() > 0) {
        QList<KDirectoryEntry> currentList;

        foreach(const KIO::UDSEntry entry, entries) {
            currentList.append(KDirectoryEntry(entry));
        }

        m_dirEntries += currentList;
        m_count = m_dirEntries.count();

        emit entriesProcessed(this);
    } else {
        qDebug() << "Entries are not added because the KDirectory object doesn't exist or no entries where received. Entries received:" << entries.count();
    }

//    foreach(KDirectoryEntry entry, m_dirEntries) {
//        qDebug() << "------------------------------";
//        qDebug() << "Entry:" << &entry;
//        qDebug() << "Name:" << entry.name();
//        qDebug() << "Basename:" << entry.basename();
//        qDebug() << "Extension:" << entry.extension();
//        qDebug() << "IconName:" << entry.iconName();
//        qDebug() << "MimeComment:" << entry.mimeComment();
//        qDebug() << "------------------------------";
//    }

    qDebug() << "Entries! Count:" << entries.count();

}

void KDirectory::slotResult(KJob *job)
{
    Q_UNUSED(job)

    // Thought: since we're emitting it directly, perhaps just remove this slot completely and emit the signal from KDirListerV2?
    emit completed(this);
}
