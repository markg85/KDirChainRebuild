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
#include <kde_file.h>

class KDirectoryEntryPrivate;

class KDirectoryEntry
{
public:

    enum DataState {
        PlainData,
        FullData
    };

    KDirectoryEntry(); // Keeps QVector happy.
    KDirectoryEntry(const KIO::UDSEntry& entry, const QString& details = "0");
    virtual ~KDirectoryEntry(){}

    const QString name() const;
    const QString basename() const;
    const QString extension() const;
    const QString iconName() const;
    const QString mimeComment() const;

    // Should be used when constructing without an USEEntry. If an UDSEntry is already present then the data will be overwritten!
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
    bool isReadable() const;

    /**
     * Checks whether the file or directory is writable. In some cases
     * (remote files), we may return true even though it can't be written to.
     * @return true if the file or directory can be written to - more precisely,
     *         false if we know for sure it can't
     */
    bool isWritable() const;

    bool isExecutable() const;
    bool isModified() const;
    bool isSystem() const;

    /**
     * Checks whether the file is hidden.
     * @return true if the file is hidden.
     */
    bool isHidden() const;

    bool entryDetailsLoaded() const;


private:
    KDirectoryEntryPrivate* d;

};

Q_DECLARE_TYPEINFO(KDirectoryEntry, Q_MOVABLE_TYPE);

#endif // KDIRECTORYENTRY_H
