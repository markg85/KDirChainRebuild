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

#ifndef KDIRLISTERV2_H
#define KDIRLISTERV2_H

#include <QObject>
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

    explicit KDirListerV2(QObject *parent = 0);

    /**
     * Run the directory lister on the given url.
     *
     * This method causes KDirLister to emit _all_ the items of @p _url, in any case.
     * Depending on _flags, either clear() or clear(const KUrl &) will be
     * emitted first.
     *
     * The newItems() signal may be emitted more than once to supply you
     * with KFileItems, up until the signal completed() is emitted
     * (and isFinished() returns true).
     *
     * @param _url     the directory URL.
     * @param _flags   whether to keep previous directories, and whether to reload, see OpenUrlFlags
     * @return true    if successful,
     *         false   otherwise (e.g. invalid @p _url)
     */
    virtual bool openUrl(const QString& url, OpenUrlFlags flags = NoFlags);

    virtual void setDetails(const QString& details);

    /**
     * This function is a convenience function to return the string (folder) that blongs to an index.
     * This makes it very easy to use this class in a model.
     *
     * @brief indexToUrl
     * @param index
     * @return
     */
    virtual const QString indexToUrl(int index);
    virtual int urlToIndex(const QString& url);
    virtual bool indexExists(const int index);
    virtual KDirectory* directory(const int index);

    
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
    void clear();
    
public slots:
    
private:
    KDirListerV2Private *const d;
};

#endif // KDIRLISTERV2_H
