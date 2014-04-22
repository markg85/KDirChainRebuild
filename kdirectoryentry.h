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

#ifndef KDIRECTORYENTRY_H
#define KDIRECTORYENTRY_H

#include <QString>
#include <QBitArray>

#include <kio/udsentry.h>
#include <kio/global.h> // for KIO::filesize_t
#include <kde_file.h>

class KDirectoryEntryPrivate;

class KDirectoryEntry
{
public:
    /**
     * The timestamps associated with a file.
     * - ModificationTime: the time the file's contents were last modified
     * - AccessTime: the time the file was last accessed (last read or written to)
     * - CreationTime: the time the file was created
     */
    enum FileTimes {
        ModificationTime,
        AccessTime,
        CreationTime
    };

    KDirectoryEntry(); // Keeps QVector happy.
    KDirectoryEntry(const KIO::UDSEntry& entry, const QString& details = "0");
    virtual ~KDirectoryEntry(){}

    /**
     * Returns the raw file name with extension as it comes from UDSEntry.
     * @return QString
     */
    const QString name() const;

    /**
     * Returns the owner of the file.
     * @return QString
     */
    const QString user() const;

    /**
     * Returns the group of the file.
     * @return QString
     */
    const QString group() const;

    /**
     * Returns the base name of a file. This is the name without extension.
     * @return QString
     */
    const QString basename() const;

    /**
     * Returns the extension of a file without leading dot.
     * @return QString
     */
    const QString extension() const;

    /**
     * Returns the icon name based on the output of QMimeType
     * @return QString
     */
    const QString iconName() const;

    /**
     * Returns the icon comment based on the output of QMimeType
     * @return QString
     */
    const QString mimeComment() const;

    /**
     * Returns the file size from UDSEntry if details where loaded. 0 for no details or if the current entry is a folder.
     * @return QString
     */
    const KIO::filesize_t size() const;

    /**
     * This is used internally when constructing this object with an UDSEntry, or used externally to update this object. For example when a rename happens. Users using this class cannot use this object to get notified of changes. The KDirectory object containing this instance will notify you of changes.
     */
    virtual void setUDSEntry(const KIO::UDSEntry& entry, const QString& details = "0");

    /**
     * Returns true if this item represents a link in the UNIX sense of
     * a link.
     * @return true if the file is a link
     */
    bool isLink() const;

    /**
     * Returns true if this item represents a directory.
     * @return true if the item is a directory
     */
    bool isDir() const;

    /**
     * Returns true if this item represents a file (and not a a directory)
     * @return true if the item is a file
     */
    bool isFile() const;

    /**
     * Checks whether the file or directory is readable. In some cases
     * (remote files), we may return true even though it can't be read.
     * @return true if the file can be read - more precisely,
     *         false if we know for sure it can't
     */
    // STUB! To be implemented
    bool isReadable() const;

    /**
     * Checks whether the file or directory is writable. In some cases
     * (remote files), we may return true even though it can't be written to.
     * @return true if the file or directory can be written to - more precisely,
     *         false if we know for sure it can't
     */
    // STUB! To be implemented
    bool isWritable() const;

    // STUB! To be implemented
    bool isExecutable() const;

    // STUB! To be implemented
    bool isModified() const;

    // STUB! To be implemented
    bool isSystem() const;

    /**
     * Requests the modification, access or creation time, depending on @p which.
     * @param which the timestamp
     * @return the time asked for, QDateTime() if not available
     * @see FileTimes
     */
    QDateTime time(FileTimes which) const;

    /**
     * Checks whether the file is hidden.
     * @return true if the file is hidden.
     */
    bool isHidden() const;

    /**
     * Checks whether the file details are loaded.
     * @return true if details are loaded, false otherwise.
     */
    bool detailsLoaded() const;

private:
    KDirectoryEntryPrivate* d;
};

Q_DECLARE_TYPEINFO(KDirectoryEntry, Q_MOVABLE_TYPE);

#endif // KDIRECTORYENTRY_H
