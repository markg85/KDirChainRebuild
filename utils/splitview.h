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
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
public:
    explicit SplitView(QQuickItem *parent = 0);

    QQmlComponent *delegate() const { return m_delegate; }
    void setDelegate(QQmlComponent *delegate);
    void distributeWidth();
    void distributeHeight();
    void elementWidthChanged();
    void splitterXChanged();

signals:
    void delegateChanged();

protected:
    void itemChange(ItemChange change, const ItemChangeData & value);
    void componentComplete();

private:
    QQmlComponent *m_delegate;
    bool m_doneInitialCreation;
};

#endif // SPLITVIEW_H
