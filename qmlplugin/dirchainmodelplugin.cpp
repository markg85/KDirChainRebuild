/*
    Copyright (C) 2014 Mark Gaiser <markg85@gmail.com>

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
#include <models/flatdirgroupedsortmodel.h>

#include <utils/breadcrumburlmodel.h>
#include <utils/urlundoredo.h>
#include <utils/shortcut.h>
#include <utils/mimeimageprovider.h>
#include <utils/splitview.h>

#include <QtQml>

void DirchainModelPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("kdirchainmodel"));
    qmlRegisterType<DirListModel>(uri, 1,0, "DirListModel");
    qmlRegisterType<DirGroupedModel>(uri, 1,0, "DirGroupedModel");
    qmlRegisterType<FlatDirGroupedSortModel>(uri, 1,0, "FlatDirGroupedSortModel");

    // Utils. QML Helper components. These should go into their own QML plugin library.
    qmlRegisterType<BreadcrumbUrlModel>(uri, 1,0, "BreadcrumbUrlModel");
    qmlRegisterType<UrlUndoRedo>(uri, 1,0, "UrlUndoRedo");
    qmlRegisterType<Shortcut>(uri, 1,0, "Shortcut");
    qmlRegisterType<SplitView>(uri, 1,0, "SplitView");
    qmlRegisterType<DirGroupedProxyModel>();
}

void DirchainModelPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
    engine->addImageProvider(QLatin1String("mime"), new MimeImageProvider);
}
