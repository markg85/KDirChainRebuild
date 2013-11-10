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

#include "dirchainmodelplugin.h"
#include <models/dirlistmodel.h>
#include <models/dirgroupedmodel.h>
#include <models/dirgroupedproxymodel.h>

#include <utils/breadcrumburlmodel.h>
#include <utils/shortcut.h>

#include <QtQml>

void DirchainModelPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("kdirchainmodel"));
    qmlRegisterType<DirListModel>(uri, 1,0, "DirListModel");
    qmlRegisterType<DirGroupedModel>(uri, 1,0, "DirGroupedModel");

    // Utils. QML Helper components. These should go into their own QML plugin library.
    qmlRegisterType<BreadcrumbUrlModel>(uri, 1,0, "BreadcrumbUrlModel");
    qmlRegisterType<Shortcut>(uri, 1,0, "Shortcut");
    qmlRegisterType<DirGroupedProxyModel>();
}
