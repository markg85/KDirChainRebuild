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

#ifndef SPLITVIEW_H
#define SPLITVIEW_H

#include <QObject>
#include <QQuickItem>

class SplitView : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQmlComponent *resizeHandleDelegate READ resizeHandleDelegate WRITE setresizeHandleDelegate NOTIFY resizeHandleDelegateChanged)
public:
    explicit SplitView(QQuickItem *parent = 0);

    QQmlComponent *resizeHandleDelegate() const { return m_resizeHandleDelegate; }
    void setresizeHandleDelegate(QQmlComponent *resizeHandleDelegate);
    void distributeWidth();
    void distributeHeight();
    void elementWidthChanged();
    void splitterXChanged();

signals:
    void resizeHandleDelegateChanged();

protected:
    void itemChange(ItemChange change, const ItemChangeData & value);

private:
    QQmlComponent *m_resizeHandleDelegate;
    bool m_insertSplitter;
};

#endif // SPLITVIEW_H
