/*
    Copyright (c) 2014, Lukas Holecek <hluk@email.cz>

    This file is part of CopyQ.

    CopyQ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CopyQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CopyQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tabbar.h"

#include "common/mimetypes.h"

#include <QMimeData>
#include <QMouseEvent>

namespace {

bool canDrop(const QMimeData &data)
{
    return data.hasFormat(mimeItems) || data.hasText() || data.hasImage() || data.hasUrls();
}

int dropItemsTabIndex(const QDropEvent &event, const QTabBar &parent)
{
    return canDrop( *event.mimeData() ) ? parent.tabAt( event.pos() ) : -1;
}

} // namespace

TabBar::TabBar(QWidget *parent)
    : QTabBar(parent)
{
    setFocusPolicy(Qt::NoFocus);
    setDrawBase(false);
    setMinimumSize(1, 1);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAcceptDrops(true);
}

void TabBar::mousePressEvent(QMouseEvent *event)
{
    bool menu = event->button() == Qt::RightButton;
    bool close = event->button() == Qt::MiddleButton;

    if (menu || close) {
        int tab = tabAt(event->pos());
        if (menu)
            emit tabMenuRequested(event->globalPos(), tab);
        else
            emit tabCloseRequested(tab);
        event->accept();
        return;
    }

    QTabBar::mousePressEvent(event);
}

void TabBar::dragEnterEvent(QDragEnterEvent *event)
{
    if ( canDrop(*event->mimeData()) )
        event->acceptProposedAction();
    else
        QTabBar::dragEnterEvent(event);
}

void TabBar::dragMoveEvent(QDragMoveEvent *event)
{
    if ( dropItemsTabIndex(*event, *this) != -1 )
        event->acceptProposedAction();
    else
        QTabBar::dragMoveEvent(event);
}

void TabBar::dropEvent(QDropEvent *event)
{
    int tabIndex = dropItemsTabIndex(*event, *this);

    if ( tabIndex != -1 ) {
        event->acceptProposedAction();
        emit dropItems( tabText(tabIndex), *event->mimeData() );
    } else {
        QTabBar::dropEvent(event);
    }
}
