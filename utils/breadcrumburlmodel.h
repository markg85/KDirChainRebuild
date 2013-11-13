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

#ifndef BREADCRUMBURLMODEL_H
#define BREADCRUMBURLMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QUrl>

class BreadcrumbUrlModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(bool hasNext READ hasNext NOTIFY urlChanged)
    Q_PROPERTY(bool hasPrevious READ hasPrevious NOTIFY urlChanged)


public:

    /**
     * @param parent parent qobject
     */
    explicit BreadcrumbUrlModel(QObject* parent = 0);
    ~BreadcrumbUrlModel();

    void setUrl(const QString& url);
    void setUrl(const QUrl& url);
    const QString url();

    // Append is meant to JUST append one new folder/file. Do NOT append multiple folders in one call.
    Q_INVOKABLE void append(QString str);

    // Parent is meant to go up one folder.
    Q_INVOKABLE void parent();

    Q_INVOKABLE void removeAfterIndex(int index);

    // Expose some QUrl functions to QML.
    Q_INVOKABLE QString protocol();
    Q_INVOKABLE QString username();
    Q_INVOKABLE QString password();
    Q_INVOKABLE int port();

    // Undo/redo support. Adding it in here is much easier then having a seperate class for it.
    Q_INVOKABLE void next();
    Q_INVOKABLE void previous();

    bool hasNext();
    bool hasPrevious();


    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    // This (re)parses the path in chunks (based on dir seperator)
    void parseUrl();

signals:
    void urlChanged();

private:
    QUrl m_url;
    QStringList m_stringList;
    QStringList m_urls;
    int m_currentUrlIndex;
};

#endif
