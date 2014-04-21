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
#include "staticmimetype.h"

// Qt includes
#include <QMimeDatabase>
#include <QMimeType>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <qplatformdefs.h>


class KDirectoryEntryPrivate
{
public:
    KDirectoryEntryPrivate()
        : m_fullUDSEntryLoaded(false)
        , m_entry()
    {
    }

    // The entry details can - technically - be loaded from within this class.
    // However, then we have a signal/slot issue. This class doesn't inherit QObject
    // and shouldn't do so since we can get A LOT of these classes in memory!
    // Remember, every file is one instance of this class.
    // So we only call this function through KDirectoryEntry::loadEntryDetails which
    // should be called from outside this class in some parent class that does have
    // QObject inheritance.
    //
    // External users of this class should first check if an entry ahs details loaded.
    // If not call KDirectory::loadEntryDetails to load the details and verify that
    // the details are actually loaded by calling KDirectoryEntry::entryDetailsChanged.

    const QString name()
    {
        return m_entry.stringValue(KIO::UDSEntry::UDS_NAME);
    }

    const QString basename()
    {
        const QString name = m_entry.stringValue(KIO::UDSEntry::UDS_NAME);
        int dotPosition = name.lastIndexOf(QStringLiteral("."));

        // If the first character is a dot then we have a file or folder that apparently wants to be hidden. Thread as basename.
        if(dotPosition == 0) {
            return name;
        }
        return name.left(dotPosition);
    }

    const QString extension()
    {
        const QString name = m_entry.stringValue(KIO::UDSEntry::UDS_NAME);

        if(!name.isEmpty() && name[0] != QChar('.')) {
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
        if(m_fullUDSEntryLoaded) {
            return false;
        }

        // To be implemented
        // const mode_t readMask = S_IRUSR|S_IRGRP|S_IROTH;

        return false;
    }

    bool isWritable()
    {
        if(m_fullUDSEntryLoaded) {
            return false;
        }

        // To be implemented
        return false;
    }

    bool isExecutable()
    {
        if(m_fullUDSEntryLoaded) {
            return false;
        }

        // To be implemented
        return false;
    }

    bool isModified()
    {
        if(m_fullUDSEntryLoaded) {
            return false;
        }

        // To be implemented
        return false;
    }

    bool isSystem()
    {
        if(m_fullUDSEntryLoaded) {
            return false;
        }

        // To be implemented
        return false;
    }

    QDateTime time(KDirectoryEntry::FileTimes which)
    {
        if(m_fullUDSEntryLoaded) {
            long long fieldVal = -1;
            switch ( which ) {
            case KDirectoryEntry::FileTimes::ModificationTime:
                fieldVal = m_entry.numberValue( KIO::UDSEntry::UDS_MODIFICATION_TIME, -1 );
                break;
            case KDirectoryEntry::FileTimes::AccessTime:
                fieldVal = m_entry.numberValue( KIO::UDSEntry::UDS_ACCESS_TIME, -1 );
                break;
            case KDirectoryEntry::FileTimes::CreationTime:
                fieldVal = m_entry.numberValue( KIO::UDSEntry::UDS_CREATION_TIME, -1 );
                break;
            }
            if (fieldVal != -1) {
                return QDateTime::fromMSecsSinceEpoch(1000 *fieldVal);
            }
        }
        return QDateTime();
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

    const StaticMimeType mimeType()
    {
        QString fName = name();
        QString ext = extension();
        if(isDir()) {
            fName = QDir::separator();
            ext = ".directory";
        }

        // This caching is based on the extension.
        static QHash<QString, StaticMimeType> mimeCache;
        if(!mimeCache.contains(ext)) {
            mimeCache.insert(ext, StaticMimeType(fName));
        }

        return mimeCache.value(ext);
    }

    const QString iconName()
    {
        return mimeType().iconName;
    }

    const QString mimeComment()
    {
        return mimeType().comment;
    }

    const KIO::filesize_t size()
    {
        if(!isDir() && m_fullUDSEntryLoaded) {
            return m_entry.numberValue(KIO::UDSEntry::UDS_SIZE);
        }
        return 0;
    }

    const QString user()
    {
        if(m_fullUDSEntryLoaded) {
            return m_entry.stringValue(KIO::UDSEntry::UDS_USER);
        }
        return QString();
    }

    const QString group()
    {
        if(m_fullUDSEntryLoaded) {
            return m_entry.stringValue(KIO::UDSEntry::UDS_GROUP);
        }
        return QString();
    }

    bool m_fullUDSEntryLoaded;
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

const QString KDirectoryEntry::user() const
{
    return d->user();
}

const QString KDirectoryEntry::group() const
{
    return d->group();
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

const KIO::filesize_t KDirectoryEntry::size() const
{
    return d->size();
}

void KDirectoryEntry::setUDSEntry(const KIO::UDSEntry &entry, const QString &details)
{
    // Details comes from the directory lister. If it's 0 then we only have very few details in the entry object.
    if(details == "0") {
        d->m_fullUDSEntryLoaded = false;
    } else {
        d->m_fullUDSEntryLoaded = true;
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

QDateTime KDirectoryEntry::time(KDirectoryEntry::FileTimes which) const
{
    return d->time(which);
}

bool KDirectoryEntry::isHidden() const
{
    return d->isHidden();
}

bool KDirectoryEntry::detailsLoaded() const
{
    return d->m_fullUDSEntryLoaded;
}
