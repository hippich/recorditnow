








// own
#include "line.h"

// Qt
#include <QResizeEvent>
#include <QMoveEvent>
#include <QDebug>


#define MIN_SIZE 20


Line::Line(const Line::Side &side)
    : QFrame(0, Qt::FramelessWindowHint|Qt::X11BypassWindowManagerHint),
    m_side(side)
{

    // init size
    switch (m_side) {
    case Left:
    case Right: resize(6, 101); break;
    case Top:
    case Bottom: resize(101, 6); break;
    }

    setMouseTracking(true);
    setFrameShadow(QFrame::Sunken);

    hide();

}



Line::~Line()
{



}


Line::Side Line::side() const
{

    return m_side;

}


void Line::mouseMoveEvent(QMouseEvent *event)
{

    // set cursor
    QRect r = rect();
    switch (m_side) {
    case Left: {
            r.setHeight(size().width());
            r.moveBottomLeft(rect().bottomLeft());
            if (r.contains(event->pos())) {
                setCursor(Qt::SizeBDiagCursor);
                break;
            }
            r.moveTopLeft(rect().topLeft());
            if (r.contains(event->pos())) {
                setCursor(Qt::SizeFDiagCursor);
                break;
            }
            if (event->buttons() == Qt::LeftButton) {
                break;
            }
            setCursor(Qt::SizeHorCursor);
            break;
        }
    case Right: {
            r.setHeight(size().width());
            r.moveBottomRight(rect().bottomRight());
            if (r.contains(event->pos())) {
                setCursor(Qt::SizeFDiagCursor);
                break;
            }
            r.moveTopLeft(rect().topLeft());
            if (r.contains(event->pos())) {
                setCursor(Qt::SizeBDiagCursor);
                break;
            }
            if (event->buttons() == Qt::LeftButton) {
                break;
            }
            setCursor(Qt::SizeHorCursor);
            break;
        }
    case Top: {/*
            r.setWidth(size().height());
            r.moveBottomRight(rect().bottomRight());
            if (r.contains(event->pos())) {
                setCursor(Qt::SizeBDiagCursor);
                break;
            }
            r.moveBottomLeft(rect().bottomLeft());
            if (r.contains(event->pos())) {
                setCursor(Qt::SizeFDiagCursor);
                break;
            }
            if (event->buttons() == Qt::LeftButton) {
                break;
            }*/
            setCursor(Qt::SizeVerCursor);
            break;
        }
    case Bottom: {/*
            r.setWidth(size().height());
            r.moveBottomRight(rect().bottomRight());
            if (r.contains(event->pos())) {
                setCursor(Qt::SizeFDiagCursor);
                break;
            }
            r.moveBottomLeft(rect().bottomLeft());
            if (r.contains(event->pos())) {
                setCursor(Qt::SizeBDiagCursor);
                break;
            }
            */
            setCursor(Qt::SizeVerCursor);
            break;
        }
    }

    // move
    if (event->buttons() == Qt::NoButton) {
        return;
    }

    const bool dia = (cursor().shape() == Qt::SizeFDiagCursor || cursor().shape() == Qt::SizeBDiagCursor);
    QRect geo = geometry();
    switch (m_side) {
    case Left: {
            if (dia) {
                if (cursor().shape() == Qt::SizeFDiagCursor) { // bottom
                    geo.setTopLeft(event->globalPos());
                } else { // top
                    geo.setBottomLeft(event->globalPos());
                }
            } else { // left/right
                geo.setLeft(event->globalX());
            }
            geo.setWidth(width());
            if (geo.height() < MIN_SIZE) {
                return;
            }
            emit geometryChanged(m_side, geo);
            break;
        }
    case Top: {
            if (dia) {
                if (cursor().shape() != Qt::SizeFDiagCursor) { // right
                    geo.setTopRight(event->globalPos());
                } else { // left
                    geo.setTopLeft(event->globalPos());
                }
            } else { // top/bottom
                geo.setTop(event->globalY());
            }
            geo.setHeight(height());
            if (geo.width() < MIN_SIZE) {
                return;
            }
            emit geometryChanged(m_side, geo);
            break;
        }
    case Right: {
            if (dia) {
                if (cursor().shape() != Qt::SizeFDiagCursor) { // bottom
                    geo.setTopLeft(event->globalPos());
                } else { // top
                    geo.setBottomLeft(event->globalPos());
                }
            } else { // left/right
                geo.setLeft(event->globalX());
            }
            geo.setWidth(width());
            if (geo.height() < MIN_SIZE) {
                return;
            }
            emit geometryChanged(m_side, geo);
            break;
        }
    case Bottom: { // TODO
            if (dia) {
                qDebug() << "diagonal move..";
                if (cursor().shape() == Qt::SizeFDiagCursor) { // right
                    qDebug() << "bottomRight" << event->globalX()-geo.bottomRight().x();
                    geo.setWidth(geo.width()-(event->globalX()-geo.bottomRight().x()));
                   // geo.setBottom(event->globalY());
                    //geo.setBottomRight(event->globalPos());
                } else { // left
                    qDebug() << "bottomLeft...";
                    geo.moveTo(event->globalPos());
                }
            } else { // top/bottom
                geo.setTop(event->globalY());
            }
            //geo.setTop(geometry().top());
            //geo.moveTop(geo.bottom()+(geometry().bottom()-geometry().top()));
            geo.setHeight(height());

            //setGeometry(geo);
            emit geometryChanged(m_side, geo);
            break;
        }
    }

}



