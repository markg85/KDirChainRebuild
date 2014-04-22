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

#ifndef KDIRECTORY_H
#define KDIRECTORY_H

#include <QObject>
#include <QDir>
#include <KIO/Job>

#include "kdirectoryentry.h"

class KDirectoryPrivate;

class KDirectory : public QObject
{
    Q_OBJECT
public:
    explicit KDirectory(const QString& directory, QObject *parent = 0);
    
    /**
     * Returns all entries that passed the filters and flags.
     * @return QVector<KDirectoryEntry>
     */
    virtual const QVector<KDirectoryEntry>& entries();

    /**
     * Entry returns the KDirectoryEntry object if it's index is in the filteredEntries.
     * @param index
     * @return KDirectoryEntry at index
     */
    virtual const KDirectoryEntry& entry(int index);

    /**
     * String of the full path for this directory.
     * @return QString
     */
    virtual const QString& url();

    /**
     * Number of entries that passed the filters and flags
     * @return int
     */
    virtual int count();

    /**
     * Set the details for the current directory. It can be 0 (no details) or 2 (full details).
     * If the value passed is different then the current details value (and valid) then the
     * directory will be re-scanned according to the new details value.
     * @param QString details. Either 0 or 2.
     * @return KDirectoryEntry at index
     */
    virtual void setDetails(const QString& details);

    // For those, see QDir documentation.
    QDir::Filters filter();
    void setFilter(QDir::Filters filters);
    QDir::SortFlags sorting();
    void setSorting(QDir::SortFlags sort);

    /**
     * Loads the entry details and passes it to the KDirectoryEntry that needs the information.
     * Be aware that this function is executing a (slow) stat call!
     * @param int id the id of the file to load. This is the QVector index id.
     */
    void loadEntryDetails(int id);

signals:
    /**
     * New entries in this folder have been processed.
     * @param KDirectory* directory pointer to the current directory. This pointer is given
     *        because you're likely to use multiple KDirectory objects so you wouldn't easily
     *        know which KDirectory object spawned this signal.
     */
    void entriesProcessed(KDirectory* dir);

    /**
     * Done loading entries.
     * @param KDirectory* directory pointer to the current directory. This pointer is given
     *        because you're likely to use multiple KDirectory objects so you wouldn't easily
     *        know which KDirectory object spawned this signal.
     */
    void completed(KDirectory* dir);

    /**
     * Done loading entries.
     * @param KDirectory* directory pointer to the current directory. This pointer is given
     *        because you're likely to use multiple KDirectory objects so you wouldn't easily
     *        know which KDirectory object spawned this signal.
     * @param int id is the id that you can use to get the KDirectoryEntry object (entry(id)).
     */
    void entryDetailsChanged(KDirectory* dir, int id);

private:
    KDirectoryPrivate *const d;
};

Q_DECLARE_TYPEINFO(KDirectory, Q_MOVABLE_TYPE);

#endif // KDIRECTORY_H
