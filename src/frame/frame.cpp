/***************************************************************************
 *   Copyright (C) 2009 by Kai Dombrowe <just89@gmx.de>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/


// own
#include "frame.h"
#include "frameinfowidget.h"
#include "movewidget.h"

// KDE
#include <kdebug.h>
#include <kwindowsystem.h>

// Qt
#include <QtGui/QResizeEvent>
#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>


#define FRAME_MIN_SIZE 100
#define PARENT_SPACING QPoint(0, 10)

Frame::Frame(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint|Qt::Tool|Qt::X11BypassWindowManagerHint)
{

    m_moveWidget = 0;

    resize(FRAME_MIN_SIZE, FRAME_MIN_SIZE);
    setContentsMargins(10, 22, 10, 22);
    setMinimumSize(FRAME_MIN_SIZE-(10*2), FRAME_MIN_SIZE-(22*2));

    if (parent) {
        parent->installEventFilter(this);
    }
    setMouseTracking(true);
    m_handle = NoHandle;
    m_active = false;

    m_infoWidget = new FrameInfoWidget(this);
    m_infoWidget->hide();

    setAttribute(Qt::WA_NoSystemBackground, true);
    m_validGeometry = false;

}


Frame::~Frame()
{

    delete m_infoWidget;
    if (m_moveWidget) {
        delete m_moveWidget;
    }

}


QRect Frame::getFrameGeometry() const
{

    QRect frameGeometry = contentsRect();
    frameGeometry.moveCenter(mapToGlobal(contentsRect().center()));
    return frameGeometry;

}


bool Frame::validFrameGeometry() const
{

    const QRect screenGeometry = qApp->desktop()->screenGeometry(this);
    return screenGeometry.contains(getFrameGeometry(), true);

}


void Frame::setFrameSize(const FrameSize &size)
{

    int top, left, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);

    resize(size.size().width()+left+right, size.size().height()+top+bottom);
    moveToParent();

}


void Frame::setVisible(bool visible)
{

    m_active = visible;
    QWidget::setVisible(visible);

}


void Frame::setView(int x, int y, int width, int height)
{

    int top, left, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    
    setGeometry(QRect(x-left, y-top, width+left+right, height+bottom+top));

}


void Frame::setZoomMode(const bool &enabled)
{

    m_infoWidget->setZoomMode(enabled);

}


void Frame::setZoomFactor(const int &factor)
{

    m_infoWidget->setZoomFactor(factor);

}


void Frame::setMoveEnabled(const bool &enabled)
{

    if (enabled) {
        if (!m_moveWidget) {
            m_moveWidget = new MoveWidget(this);
        }
    } else {
        if (m_moveWidget) {
            delete m_moveWidget;
            m_moveWidget = 0;
        }
    }
    moveToParent(true);

}


QRect Frame::getRect(const ResizeHandle &handle) const
{

    const int size = getLineSize();
    switch (handle) {
    case TopLeft: return QRect(0, 0, size, size);
    case TopRight: return QRect(rect().topRight()-QPoint(size, 0), QSize(size, size));
    case BottomLeft: return QRect(rect().bottomLeft()-QPoint(0, size), QSize(size, size));
    case BottomRight: return QRect(rect().bottomRight()-QPoint(size, size), QSize(size, size));
    case Left: return QRect(0, 0, size, height());
    case Right: return QRect(rect().topRight()-QPoint(size, 0), QSize(size, height()));
    case Top: return QRect(0, 0, width(), size);
    case Bottom: return QRect(rect().bottomLeft()-QPoint(0, size), QSize(width(), size));
    default: return QRect();
    }

}


int Frame::getLineSize() const
{

    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    return top;

}


void Frame::moveToParent(const bool &force)
{

    if (!parentWidget() || (m_moveWidget && !force)) {
        return;
    }

    QRect parentGeometry = parentWidget()->frameGeometry();
    // geometry() returns sometimes/always x(0) y(0) if the window has not yet moved
    if (parentGeometry.x() < 1 && parentGeometry.y() < 1) {
        const KWindowInfo info = KWindowSystem::self()->windowInfo(parentWidget()->winId(),
                                                                   NET::WMFrameExtents);
        parentGeometry = info.frameGeometry();
    }

    QRect newGeometry = geometry();
    newGeometry.moveTopLeft(parentGeometry.bottomLeft()+PARENT_SPACING);
    setGeometry(newGeometry);

}


void Frame::moveParentToFrame()
{

    if (!parentWidget() || m_moveWidget) {
        return;
    }

    QRect geometry = parentWidget()->frameGeometry();
    const QPoint pos = geometry.topLeft()-parentWidget()->geometry().topLeft();
    const QSize size = geometry.size()-parentWidget()->geometry().size();

    geometry.moveBottomLeft(this->geometry().topLeft()-PARENT_SPACING);
    geometry.moveTopLeft(geometry.topLeft()-pos);
    geometry.setSize(geometry.size()-size);

    parentWidget()->setGeometry(geometry);

}


void Frame::show()
{

    QWidget::setVisible(true);

}


void Frame::hide()
{

    QWidget::setVisible(false);

}


bool Frame::eventFilter(QObject *watched, QEvent *event)
{

    if (m_handle == NoHandle) {
        if (watched == parent()) {
            switch (event->type()) {
            case QEvent::Move:
            case QEvent::Resize: moveToParent(); break;
            case QEvent::WindowDeactivate: {
                    if (m_active && !m_moveWidget) {
                        hide();
                    }
                    break;
                }
            case QEvent::WindowActivate: {
                    if (m_active && !m_moveWidget) {
                        show();
                    }
                    break;
                }
            default: break;
            }
        }
    }
    return QWidget::eventFilter(watched, event);

}


void Frame::resizeEvent(QResizeEvent *event)
{

    QWidget::resizeEvent(event);
    setMask(QRegion(contentsRect()).xored(QRegion(rect())));
    if (validFrameGeometry() != m_validGeometry) {
        m_validGeometry = !m_validGeometry;
        update();
    }

}


void Frame::mouseMoveEvent(QMouseEvent *event)
{

    if (m_handle == NoHandle) { // set cursor
        if (getRect(TopLeft).contains(event->pos())) {
            setCursor(Qt::SizeFDiagCursor);
        } else if (getRect(TopRight).contains(event->pos())) {
            setCursor(Qt::SizeBDiagCursor);
        } else if (getRect(BottomLeft).contains(event->pos())) {
            setCursor(Qt::SizeBDiagCursor);
        } else if (getRect(BottomRight).contains(event->pos())) {
            setCursor(Qt::SizeFDiagCursor);
        } else if (getRect(Left).contains(event->pos())) {
            setCursor(Qt::SizeHorCursor);
        } else if (getRect(Right).contains(event->pos())) {
            setCursor(Qt::SizeHorCursor);
        } else if (getRect(Top).contains(event->pos())) {
            setCursor(Qt::SizeVerCursor);
        } else if (getRect(Bottom).contains(event->pos())) {
            setCursor(Qt::SizeVerCursor);
        }
    } else { // resize
        QRect geometry = this->geometry();
        QRect tmp = geometry;
        switch (m_handle) {
        case Left: {
                tmp.setLeft(event->globalX());
                if (tmp.width() < FRAME_MIN_SIZE) {
                    tmp.setRight(geometry.right());
                    tmp.setLeft(tmp.right()-FRAME_MIN_SIZE);
                }
                geometry = tmp;
                break;
            }
        case Right: {
                tmp.setRight(event->globalX());
                if (tmp.width() < FRAME_MIN_SIZE) {
                    tmp.setLeft(geometry.left());
                    tmp.setRight(tmp.left()+FRAME_MIN_SIZE);
                }
                geometry = tmp;
                break;
            }
        case Top:  {
                tmp.setY(event->globalY());
                if (tmp.height() < FRAME_MIN_SIZE) {
                    tmp.setBottom(geometry.bottom());
                    tmp.setTop(tmp.bottom()-FRAME_MIN_SIZE);
                }
                geometry = tmp;
                break;
            }
        case Bottom: {
                tmp.setBottom(event->globalY());
                if (tmp.height() < FRAME_MIN_SIZE) {
                    tmp.setTop(geometry.top());
                    tmp.setBottom(tmp.top()+FRAME_MIN_SIZE);
                }
                geometry = tmp;
                break;
            }
        case TopLeft: {
                tmp.setTopLeft(event->globalPos());
                if (tmp.height() < FRAME_MIN_SIZE) {
                    tmp.setBottom(geometry.bottom());
                    tmp.setTop(tmp.bottom()-FRAME_MIN_SIZE);
                }

                if (tmp.width() < FRAME_MIN_SIZE) {
                    tmp.setRight(geometry.right());
                    tmp.setLeft(tmp.right()-FRAME_MIN_SIZE);
                }
                geometry = tmp;
                break;
            }
        case TopRight:  {
                tmp.setTopRight(event->globalPos());
                if (tmp.height() < FRAME_MIN_SIZE) {
                    tmp.setBottom(geometry.bottom());
                    tmp.setTop(tmp.bottom()-FRAME_MIN_SIZE);
                }

                if (tmp.width() < FRAME_MIN_SIZE) {
                    tmp.setLeft(geometry.left());
                    tmp.setRight(tmp.left()+FRAME_MIN_SIZE);
                }
                geometry = tmp;
                break;
            }
        case BottomLeft:  {
                tmp.setBottomLeft(event->globalPos());
                if (tmp.height() < FRAME_MIN_SIZE) {
                    tmp.setTop(geometry.top());
                    tmp.setBottom(tmp.top()+FRAME_MIN_SIZE);
                }

                if (tmp.width() < FRAME_MIN_SIZE) {
                    tmp.setRight(geometry.right());
                    tmp.setLeft(tmp.right()-FRAME_MIN_SIZE);
                }
                geometry = tmp;
                break;
            }
        case BottomRight:  {
                tmp.setBottomRight(event->globalPos());
                if (tmp.height() < FRAME_MIN_SIZE) {
                    tmp.setTop(geometry.top());
                    tmp.setBottom(tmp.top()+FRAME_MIN_SIZE);
                }

                if (tmp.width() < FRAME_MIN_SIZE) {
                    tmp.setLeft(geometry.left());
                    tmp.setRight(tmp.left()+FRAME_MIN_SIZE);
                }
                geometry = tmp;
                break;
            }
        default: break;
        }

        setGeometry(geometry);
        moveParentToFrame();
    }

}


void Frame::mousePressEvent(QMouseEvent *event)
{

    if (event->button() & Qt::LeftButton) {
        if (getRect(TopLeft).contains(event->pos())) {
            m_handle = TopLeft;
        } else if (getRect(TopRight).contains(event->pos())) {
            m_handle = TopRight;
        } else if (getRect(BottomLeft).contains(event->pos())) {
            m_handle = BottomLeft;
        } else if (getRect(BottomRight).contains(event->pos())) {
            m_handle = BottomRight;
        } else if (getRect(Left).contains(event->pos())) {
            m_handle = Left;
        } else if (getRect(Right).contains(event->pos())) {
            m_handle = Right;
        } else if (getRect(Top).contains(event->pos())) {
            m_handle = Top;
        } else if (getRect(Bottom).contains(event->pos())) {
            m_handle = Bottom;
        } else {
            m_handle = NoHandle;
        }
    } else {
        m_handle = NoHandle;
    }

    if (m_handle != NoHandle) {
        grabMouse();
    }

}


void Frame::mouseReleaseEvent(QMouseEvent *event)
{

    Q_UNUSED(event);
    m_handle = NoHandle;
    releaseMouse();

}


void Frame::showEvent(QShowEvent *event)
{

    moveToParent();
    QWidget::showEvent(event);

}


void Frame::moveEvent(QMoveEvent *event)
{

    QWidget::moveEvent(event);
    if (validFrameGeometry() != m_validGeometry) {
        m_validGeometry = !m_validGeometry;
        update();
    }

}


void Frame::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    painter.setClipRegion(event->region());
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QPen pen;
    pen.setColor(Qt::lightGray);
    pen.setWidth(2);

    QRect left = rect();
    left.setRight(contentsRect().left());

    QRect right = left;
    right.moveTopRight(rect().topRight());

    QRect top = rect();
    top.setBottom(contentsRect().top());

    QRect bottom = top;
    bottom.moveTop(contentsRect().bottom());

    painter.fillRect(left, m_validGeometry ? Qt::black : Qt::red);
    painter.fillRect(right, m_validGeometry ? Qt::black : Qt::red);
    painter.fillRect(top, m_validGeometry ? Qt::black : Qt::red);
    painter.fillRect(bottom, m_validGeometry ? Qt::black : Qt::red);

    QRect box = QRect(0, 0, (top.height()/2)-(pen.width()*2), (top.height()/2)-(pen.width()*2));
    box.moveCenter(top.center());
    const int spacing = 10;

    QRect contents = top;
    contents.setWidth(contents.width()-(pen.width()*2));
    contents.moveLeft(contents.left()-pen.width());

    painter.setBrush(Qt::white);
    painter.setPen(Qt::white);
    for (int i = spacing; i < top.width(); i += spacing+box.width()) {
        box.moveLeft(i);

        if (!contents.contains(box, true)) {
            break;
        }

        painter.drawRoundedRect(box, 0.5, 0.5);
    }

    box = QRect(0, 0, (bottom.height()/2)-(pen.width()*2), (bottom.height()/2)-(pen.width()*2));
    box.moveCenter(bottom.center());

    contents = bottom;
    contents.setWidth(contents.width()-(pen.width()*2));
    contents.moveLeft(contents.left()-pen.width());

    for (int i = spacing; i < bottom.width(); i += spacing+box.width()) {
        box.moveLeft(i);

        if (!contents.contains(box, true)) {
            break;
        }

        painter.drawRoundedRect(box, 0.5, 0.5);
    }

    painter.setBrush(QBrush());
    painter.setPen(pen);

    QRect in = contentsRect();
    in.setHeight(in.height()+(painter.pen().width()*2));
    in.setWidth(in.width()+(pen.width()*2));
    in.moveTopLeft(in.topLeft()-QPoint(painter.pen().width(), painter.pen().width()));
    painter.drawRect(in);

    QRect out = rect();
    out.setHeight(out.height()-(painter.pen().width()*2));
    out.setWidth(out.width()-(painter.pen().width()*2));
    out.moveTopLeft(rect().topLeft()+QPoint(painter.pen().width(), painter.pen().width()));
    painter.drawRect(out);

}


#include "frame.moc"

