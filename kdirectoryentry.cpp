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

#include "kdirectoryentry.h"

// Qt includes
#include <QMimeDatabase>
#include <QMimeType>
#include <QDebug>
#include <qplatformdefs.h>


class KDirectoryEntryPrivate
{
public:
    KDirectoryEntryPrivate()
        : m_dataState(KDirectoryEntry::DataState::PlainData)
        , m_entry()
    {
    }

    // The entry details can - technically - be loaded from within this class.
    // However, then we have a signal/slot issue. This class doesn't inherit QObject
    // and shouldn't do so since we can get A LOT of these classes in memory!
    // Remember, every file is one instance of this class.
    // So we only call this function through KDirectoryEntry::loadEntryDetails which
    // should be called from outside this class in some parent class that does have
    // QObject inheritance. In KDirectory would be ideal.
    // External users of this class should first check of an entry ahs details loaded.
    // If not call KDirectory::loadEntryDetails to load the details and verify that
    // the details are actually loaded by calling KDirectoryEntry::entryDetailsLoaded.

    const QString name()
    {
        return m_entry.stringValue(KIO::UDSEntry::UDS_NAME);
    }

    const QString basename()
    {
        const QString name = m_entry.stringValue(KIO::UDSEntry::UDS_NAME);
//        m_fileMode = m_entry.numberValue(KIO::UDSEntry::UDS_FILE_TYPE);


        int dotPosition = name.lastIndexOf(QStringLiteral("."));
        return name.left(dotPosition);
    }

    const QString extension()
    {
        const QString name = m_entry.stringValue(KIO::UDSEntry::UDS_NAME);

        if(name[0] != QChar('.')) {
            int lastDot = name.lastIndexOf(QStringLiteral("."));

            if(lastDot > 0) {
                int dotPosition = name.length() - lastDot - 1;
                return name.right(dotPosition);
            }
        }
        return QString();
    }

    bool isReadable()
    {
        if(m_dataState != KDirectoryEntry::DataState::FullData) {
            return false;
        }

        // To be implemented
        // const mode_t readMask = S_IRUSR|S_IRGRP|S_IROTH;

        return false;
    }

    bool isWritable()
    {
        return false;
    }

    bool isExecutable()
    {
        return false;
    }

    bool isModified()
    {
        return false;
    }

    bool isSystem()
    {
        return false;
    }


    bool isHidden()
    {
        const QString fname = m_entry.stringValue(KIO::UDSEntry::UDS_NAME);
        if(fname.length() > 0 && fname[0] == '.') {
            return true;
        } else {
            return false;
        }
    }

    bool isDir()
    {
        return (m_entry.numberValue(KIO::UDSEntry::UDS_FILE_TYPE) & QT_STAT_MASK) == QT_STAT_DIR;
    }

    const QMimeType mimeType(const QString& ext)
    {
        // This could be optimized!
        // We should perhaps have a singlethon where we register the extension with the mime object.
        // That does require some bookkeeping.. For now we keep it "dumb"

        QMimeDatabase db;
        QMimeType mime = db.mimeTypeForFile(ext, QMimeDatabase::MatchExtension);
        return mime;
    }

    const QString iconName()
    {
        const QMimeType& mime = mimeType(extension());

        if(mime.isValid()) {
            return mime.iconName();
        }
        return QString();
    }

    const QString mimeComment()
    {
        const QMimeType& mime = mimeType(extension());

        if(mime.isValid()) {
            return mime.comment();
        }
        return QString();
    }

    bool m_dataState;
    KIO::UDSEntry m_entry;
};

KDirectoryEntry::KDirectoryEntry()
    : d(new KDirectoryEntryPrivate())
{
}

KDirectoryEntry::KDirectoryEntry(const KIO::UDSEntry &entry, const QString &details)
    : d(new KDirectoryEntryPrivate())
{
    setUDSEntry(entry, details);
}

const QString KDirectoryEntry::name() const
{
    return d->name();
}

const QString KDirectoryEntry::basename() const
{
    return d->basename();
}

const QString KDirectoryEntry::extension() const
{
    return d->extension();
}

const QString KDirectoryEntry::iconName() const
{

    return d->iconName();
}

const QString KDirectoryEntry::mimeComment() const
{

    return d->mimeComment();
}

void KDirectoryEntry::setUDSEntry(const KIO::UDSEntry &entry, const QString &details)
{
    // Details comes from the directory lister. If it's 0 then we only have very few details in the entry object.
    if(details == "0") {
        d->m_dataState = KDirectoryEntry::DataState::PlainData;
    } else {
        d->m_dataState = KDirectoryEntry::DataState::FullData;
    }
    d->m_entry = entry;
}

bool KDirectoryEntry::isLink() const
{
    return d->m_entry.contains(KIO::UDSEntry::UDS_LINK_DEST); // Link location is not stored, only that it is a link
}

bool KDirectoryEntry::isDir() const
{
    return d->isDir();
}

bool KDirectoryEntry::isFile() const
{
    return !isDir();
}

bool KDirectoryEntry::isReadable() const
{
    return d->isReadable();
}

bool KDirectoryEntry::isWritable() const
{
    return d->isWritable();
}

bool KDirectoryEntry::isExecutable() const
{
    return d->isExecutable();
}

bool KDirectoryEntry::isModified() const
{
    return d->isModified();
}

bool KDirectoryEntry::isSystem() const
{
    return d->isSystem();
}

bool KDirectoryEntry::isHidden() const
{
    return d->isHidden();
}

bool KDirectoryEntry::entryDetailsLoaded() const
{
    if(d->m_dataState == KDirectoryEntry::DataState::FullData) {
        return true;
    } else {
        return false;
    }
}
