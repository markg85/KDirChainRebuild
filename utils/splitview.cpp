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

#include "splitview.h"

#include <QDebug>

SplitView::SplitView(QQuickItem *parent)
    : QQuickItem(parent)
    , m_delegate(0)
    , m_doneInitialCreation(false)
{
    connect(this, &QQuickItem::widthChanged, this, &SplitView::distributeWidth);
    connect(this, &QQuickItem::heightChanged, this, &SplitView::distributeHeight);
}

void SplitView::setDelegate(QQmlComponent *delegate)
{
    if (delegate != m_delegate) {
        m_delegate = delegate;
        emit delegateChanged();
    }
}

void SplitView::distributeWidth()
{
    int numOfItemsFlexible = 0;
    qreal fixedWidth = 0.0;

    // First pass, figure out the items that need resizing and how much width is fixed.
    for(QQuickItem* item : childItems()) {
        item->blockSignals(true);
        if(item->property("dynamicWidth").isValid()) {
            numOfItemsFlexible++;
        } else {
            fixedWidth += item->width();
        }
    }

    // Second pass, resize the items.
    qreal xOffset = 0.0;

    const qreal widthPerItem = (width() - fixedWidth) / numOfItemsFlexible;
    for(QQuickItem* item : childItems()) {
        if(item->property("dynamicWidth").isValid()) {
            item->setImplicitWidth(widthPerItem);
        }

        item->setX(xOffset);
        xOffset += item->width();
        item->blockSignals(false);
    }

}

void SplitView::distributeHeight()
{
    // not much to distribute. Just set the new height.
    for(QQuickItem* item : childItems()) {
        item->setHeight(height());
    }
}

void SplitView::elementWidthChanged()
{
    QQuickItem *item = qobject_cast<QQuickItem*>(sender());

    const int index = childItems().indexOf(item);

    // If we are the last item then just allow the resize.
    if(index == childItems().count() - 1) {
        return;
    }

    // The next item should be a splitter one. Get it and change it's X. splitterXChanged will then pick it up and do the rest.
    QQuickItem* splitter = childItems().at(index + 1);
    splitter->setX(item->x() + item->width());
}

void SplitView::splitterXChanged()
{
    if(!m_doneInitialCreation) {
        return;
    }

    QQuickItem *splitterItem = qobject_cast<QQuickItem*>(sender());
    const int index = childItems().indexOf(splitterItem);

    // If we have an index with 0 (or smaller) then we don't have any items before it so return.
    if(index <= 0) {
        return;
    }

    QQuickItem *leftItem = qobject_cast<QQuickItem*>(childItems().at(index - 1));

    const qreal newWidthLeft = splitterItem->x() - leftItem->x();
    leftItem->setWidth(newWidthLeft);

    QQuickItem *rightItem = qobject_cast<QQuickItem*>(childItems().at(index + 1));
    const qreal newWidthRight = (rightItem->x() + rightItem->width()) - (splitterItem->x() + splitterItem->width());
    rightItem->setX(splitterItem->x() + splitterItem->width());
    rightItem->setWidth(newWidthRight);
}

void SplitView::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value)
{
    QQuickItem::itemChange(change, value);
}

void SplitView::componentComplete()
{
    QQuickItem::componentComplete();

    for(QQuickItem* item : childItems()) {
        if(item->width() == 0) {
            item->setProperty("dynamicWidth", true);
        }
    }

    // Inject splitters
    const int numOfSplitters = childItems().count() - 1;
    for(int i = 0; i < numOfSplitters; i++) {
        const int injectAfterId = i + i;
        QObject *myObject = m_delegate->create();
        QQuickItem *temp = qobject_cast<QQuickItem*>(myObject);
        temp->setParentItem(this);
        temp->stackAfter(childItems().at(injectAfterId));
        temp->setProperty("splitter", true);

        connect(temp, &QQuickItem::xChanged, this, &SplitView::splitterXChanged);
    }

    // Coonect widthChanged signal of all components
    for(QQuickItem* item : childItems()) {
        connect(item, &QQuickItem::widthChanged, this, &SplitView::elementWidthChanged);
    }

    distributeWidth();
    distributeHeight();

    m_doneInitialCreation = true;
}
