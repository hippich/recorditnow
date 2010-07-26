/***************************************************************************
 *   Copyright (C) 2010 by Kai Dombrowe <just89@gmx.de>                    *
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
#include "keyloggerwidget.h"
#include "keyloggerlabel.h"

// KDE
#include <plasma/theme.h>
#include <plasma/framesvg.h>
#include <kwindowsystem.h>

// Qt
#include <QtGui/QResizeEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>
#include <QtCore/QTimer>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QDebug>
#include <QtGui/QLineEdit>
#include <QtGui/QHBoxLayout>
#include <QtGui/QX11Info>

// X11
#include <X11/Xlib.h>
#include <X11/extensions/shape.h>


namespace RecordItNow {



KeyloggerWidget::KeyloggerWidget(QWidget *parent)
    : QFrame(parent, Qt::Window|
              Qt::FramelessWindowHint|
              Qt::X11BypassWindowManagerHint|
              Qt::WindowStaysOnTopHint)
{

    setAttribute(Qt::WA_TranslucentBackground, true);

    m_background = new Plasma::FrameSvg(this);
    m_background->setImagePath("widgets/tooltip");

    m_animation = new QPropertyAnimation(this);
    m_animation->setPropertyName("windowOpacity");
    m_animation->setTargetObject(this);
    m_animation->setEasingCurve(QEasingCurve::Linear);
    m_animation->setDuration(300);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);

    m_edit = new KeyloggerLabel(this);
    m_edit->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(m_edit);
    setLayout(layout);

    connect(m_background, SIGNAL(repaintNeeded()), this, SLOT(backgroundChanged()));

    connect(qApp->desktop(), SIGNAL(workAreaResized(int)), this,
            SLOT(screenGeometryChanged(int)));

    backgroundChanged();
    screenGeometryChanged(-1);


    m_timer = new QTimer(this);
    m_timer->setInterval(1000/2);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateMousePos()));

    m_hideTimer = new QTimer(this);
    m_hideTimer->setSingleShot(true);
    m_hideTimer->setInterval(5*1000); // hide after 5 sec
    connect(m_hideTimer, SIGNAL(timeout()), this, SLOT(inactive()));


    int junk;
    if (XQueryExtension(x11Info().display(), "SHAPE", &junk, &junk, &junk)) {
        Pixmap mask = XCreatePixmap(x11Info().display(), winId(), 1, 1, 1);
        XShapeCombineMask(x11Info().display(), winId(), ShapeInput, 0, 0, mask, ShapeSet);
        XFreePixmap(x11Info().display(), mask);
    }

    m_timer->start();

    init(5, 30, 500);

}


KeyloggerWidget::~KeyloggerWidget()
{

    delete m_hideTimer;
    delete m_timer;
    delete m_animation;
    delete m_background;

}


void KeyloggerWidget::init(const int &timeout, const int &fontSize, const int &width)
{

    m_hideTimer->setInterval(timeout*1000);

    QFont font = QFrame::font();
    if (m_background->isValid()) {
        font = Plasma::Theme::defaultTheme()->font(Plasma::Theme::DefaultFont);
    }
    font.setPointSize(fontSize);
    m_edit->setFont(font);

    QFontMetrics fm(font);
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);

    setMinimumWidth(width);
    setMinimumHeight(fm.height()+top+bottom+5);

}


void KeyloggerWidget::updateMousePos()
{

    if (!isVisible()) {
        return;
    }

    if (geometry().contains(QCursor::pos())) {
        if (!KWindowSystem::self()->compositingActive()) {
            hide();
        } else {
            if (m_animation->state() == QPropertyAnimation::Running) {
                if (m_animation->endValue().toInt() == 1) {
                    m_animation->stop();
                } else {
                    return;
                }
            }
            m_animation->setStartValue(windowOpacity());
            m_animation->setEndValue(0.1);

            m_animation->start();
        }
    } else {
        if (!KWindowSystem::self()->compositingActive()) {
            hide();
        } else {
            if (m_animation->state() == QPropertyAnimation::Running) {
                if (m_animation->endValue().toInt() != 1) {
                    m_animation->stop();
                } else {
                    return;
                }
            }
            m_animation->setStartValue(windowOpacity());
            m_animation->setEndValue(1.0);

            m_animation->start();
        }
    }

}


void KeyloggerWidget::screenGeometryChanged(const int &screen)
{

    Q_UNUSED(screen);
    updateGeometry();

}


void KeyloggerWidget::backgroundChanged()
{

    if (m_background->isValid()) {
        qreal left, top, right, bottom;
        m_background->getMargins(left, top, right, bottom);
        setContentsMargins(left, top, right, bottom);
    }

}


void KeyloggerWidget::updateGeometry()
{

    const QRect desktopGeometry =  qApp->desktop()->screenGeometry(this);
    QRect newGeometry = geometry();

    newGeometry.setSize(minimumSize());
    newGeometry.moveTopLeft(desktopGeometry.topLeft()+QPoint(10, 10));

    setGeometry(newGeometry);

}


void KeyloggerWidget::inactive()
{

    hide();

}


void KeyloggerWidget::keyPressEvent(QKeyEvent *event)
{

    if (m_hideTimer->isActive()) {
        m_hideTimer->stop();
    }

    if (!isVisible()) {
        show();
        updateMousePos();
    }
    m_edit->pressEvent(event);

}


void KeyloggerWidget::keyReleaseEvent(QKeyEvent *event)
{

    m_edit->releaseEvent(event);
    if (m_hideTimer->isActive()) {
        m_hideTimer->stop();
    }
    m_hideTimer->start();

}


void KeyloggerWidget::resizeEvent(QResizeEvent *event)
{

    QWidget::resizeEvent(event);
    if (m_background->isValid()) {
        m_background->resizeFrame(event->size());
        setMask(m_background->mask());
    }

}


void KeyloggerWidget::paintEvent(QPaintEvent *event)
{

    if (m_background->isValid()) {
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
        painter.setClipRegion(event->region());

        m_background->paintFrame(&painter);
    } else {
        QFrame::paintEvent(event);
    }

}


} // namespace RecordItNow


#include "keyloggerwidget.moc"
