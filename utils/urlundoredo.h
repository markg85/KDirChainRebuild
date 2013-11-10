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

#ifndef URLUNDOREDO_H
#define URLUNDOREDO_H

#include <QObject>
#include <QStringList>

class UrlUndoRedo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentUrl READ currentUrl NOTIFY currentUrlChanged)
    Q_PROPERTY(bool hasNext READ hasNext NOTIFY currentUrlChanged)
    Q_PROPERTY(bool hasPrevious READ hasPrevious NOTIFY currentUrlChanged)
public:
    explicit UrlUndoRedo(QObject *parent = 0);
    Q_INVOKABLE void add(const QString& url);
    Q_INVOKABLE void next();
    Q_INVOKABLE void previous();

    QString currentUrl();

    bool hasNext();
    bool hasPrevious();
    
signals:
    void currentUrlChanged();
    
public slots:
    
private:
    QStringList m_urls;
    int m_currentUrlIndex;
};

#endif // URLUNDOREDO_H
