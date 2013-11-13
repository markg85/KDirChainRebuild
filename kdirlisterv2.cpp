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

#include "kdirlisterv2.h"
#include "kdirlisterv2_p.h"

KDirListerV2::KDirListerV2(QObject *parent)
    : QObject(parent)
    , d(new KDirListerV2Private(this))
{
    // Forward signals from the private class to the public class
    connect(d, SIGNAL(completed(KDirectory*)), this, SIGNAL(completed(KDirectory*)));
    connect(d, SIGNAL(directoryContentChanged(KDirectory*)), this, SIGNAL(directoryContentChanged(KDirectory*)));
}

bool KDirListerV2::openUrl(const QString &url, OpenUrlFlags flags)
{
    // Whatever the URL or Flags might be, pass it along to the private class. It will determine what to do.
    d->addUrl(url, flags);

    return true;
}

bool KDirListerV2::openUrl(KDirListerV2::DirectoryFetchDetails dirFetchDetails)
{
    d->addUrl(dirFetchDetails);

    return true;
}

bool KDirListerV2::isListing(const QString &url)
{
    return d->isListing(url);
}

KDirectory *KDirListerV2::directory(const QString &url)
{
    return d->directory(url);
}
