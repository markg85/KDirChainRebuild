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

#ifndef KDIRLISTERV2_H
#define KDIRLISTERV2_H

#include <QObject>
#include <QDir>
#include "kdirectory.h"

class KDirListerV2Private;

class KDirListerV2 : public QObject
{
    Q_OBJECT
public:
    enum OpenUrlFlag
    {
      NoFlags = 0x0,   ///< No additional flags specified.

      Keep = 0x1,      ///< Previous directories aren't forgotten
                       ///< (they are still watched by kdirwatch and their items
                       ///< are kept for this KDirLister). This is useful for e.g.
                       ///< a treeview.

      Reload = 0x2     ///< Indicates whether to use the cache or to reread
                       ///< the directory from the disk.
                       ///< Use only when opening a dir not yet listed by this lister
                       ///< without using the cache. Otherwise use updateDirectory.
    };

    Q_DECLARE_FLAGS(OpenUrlFlags, OpenUrlFlag)

    struct DirectoryFetchDetails {
        QString url;
        QString details = "0";
        QDir::Filters filters = QDir::NoFilter;
        QDir::SortFlags sorting = QDir::NoSort;
        KDirListerV2::OpenUrlFlags openFlags = OpenUrlFlag::NoFlags;
    };


    explicit KDirListerV2(QObject *parent = 0);

    /**
     * Open a new URL to be listed.
     *
     * @param url the directory URL.
     * @param flags. Can be NoFlags, Keep or Reload
     * @return true if successful, false otherwise.
     */
    virtual bool openUrl(const QString& url, OpenUrlFlags flags = NoFlags);

    /**
     * Open a new URL to be listed.
     *
     * @param dirFetchDetails
     * @return true if successful, false otherwise.
     * @see DirectoryFetchDetails
     */
    virtual bool openUrl(DirectoryFetchDetails dirFetchDetails);

    /**
     * Test if a given URL is being listed.
     *
     * @return true if directory is listed, false otherwise
     */
    virtual bool isListing(const QString& url);

    /**
     * Return a KDirectory pointer representing the given URL.
     *
     * @param url
     * @return KDirectory pointer if url is known, nullptr otherwise.
     */
    virtual KDirectory* directory(const QString& url);
    
signals:
    /**
     * NOTE: pay close attention here, This signal is returning the internal
     * KDirectory object which has ALL entries inside up till the emit moment!
     * It does not emit incremental updates!
     *
     * Also, there is no guarantee that this pointer will be the same for the
     * same folder. You should therefore not rely on this pointer.
     *
     * @param items
     */
    void directoryContentChanged(KDirectory* directoryContent);

    /**
     * @brief completed
     * @param items
     */
    void completed(KDirectory* directoryContent);
    
public slots:
    
private:
    KDirListerV2Private *const d;
};

#endif // KDIRLISTERV2_H
