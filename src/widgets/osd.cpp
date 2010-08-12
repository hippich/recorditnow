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
#include "osd.h"

// KDE
#include <plasma/theme.h>
#include <plasma/framesvg.h>
#include <kwindowsystem.h>
#include <kdeversion.h>
#if KDE_VERSION_MINOR >= 5
    #include <plasma/windoweffects.h>
#endif
#include <kdebug.h>
#include <kglobalsettings.h>

// Qt
#include <QtGui/QResizeEvent>
#include <QtGui/QPainter>
#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>
#include <QtGui/QX11Info>

// X11
#include <X11/Xlib.h>
#include <X11/extensions/shape.h>


namespace RecordItNow {


OSD::OSD(QWidget *parent)
    : QFrame(parent, Qt::Window|
             Qt::FramelessWindowHint|
             Qt::X11BypassWindowManagerHint|
             Qt::WindowStaysOnTopHint)
{

    m_background = new Plasma::FrameSvg(this);
    m_validBackground = m_blurEnabled = m_transparentForMouseEvents = false;

    connect(Plasma::Theme::defaultTheme(), SIGNAL(themeChanged()), this, SLOT(backgroundChanged()));
    connect(m_background, SIGNAL(repaintNeeded()), this, SLOT(backgroundChanged()));
    connect(qApp->desktop(), SIGNAL(workAreaResized(int)), this,
            SLOT(screenGeometryChanged(int)));

    backgroundChanged();

}


OSD::~OSD()
{

    delete m_background;

}


QVariant  OSD::saveGeometry(QWidget *parent, const QRect &geometry)
{

    QRectF p;

    const QRect screenGeometry = qApp->desktop()->screenGeometry(parent);
    p.setX(float(geometry.x())/float((screenGeometry.right()/100)));
    p.setY(float(geometry.y())/float((screenGeometry.bottom()/100)));
    p.setWidth(float(geometry.width())/float((screenGeometry.width()/100)));
    p.setHeight(float(geometry.height())/float((screenGeometry.height()/100)));

    return QVariant(p);

}


void OSD::setTransparentForMouseEvents(const bool &transparent)
{

    int junk = 0;
    if (m_transparentForMouseEvents && !transparent) {
        if (XQueryExtension(x11Info().display(), "SHAPE", &junk, &junk, &junk)) {
            XShapeCombineMask(x11Info().display(), winId(), ShapeInput, 0, 0, None, ShapeSet);
        }
    } else if (!m_transparentForMouseEvents && transparent) {
        if (XQueryExtension(x11Info().display(), "SHAPE", &junk, &junk, &junk)) {
            Pixmap mask = XCreatePixmap(x11Info().display(), winId(), 1, 1, 1);
            XShapeCombineMask(x11Info().display(), winId(), ShapeInput, 0, 0, mask, ShapeSet);
            XFreePixmap(x11Info().display(), mask);
        }
    }
    m_transparentForMouseEvents = transparent;

}


void OSD::setBackgroundImage(const QString &imagePath)
{

    m_background->setImagePath(imagePath);

}


void OSD::setBackgroundElementPrefix(const QString &prefix)
{

    m_background->setElementPrefix(prefix);

}


void OSD::setBlurEnabled(const bool &enabled)
{

    m_blurEnabled = enabled;
    updateBlur();

}


void OSD::loadGeometry(const QVariant &geometry)
{

    setOSDGeometry(geometry.toRectF());

}


void OSD::setOSDGeometry(const QRectF &geometry)
{

    m_osdGeometry = geometry;
    if (m_osdGeometry == QRectF(-1, -1, -1, -1)) {
        m_osdGeometry = saveGeometry(this, QRect(10, 10, 600, 200)).toRectF();
    }


    const QRect screenGeometry = qApp->desktop()->screenGeometry(this);
    QRect windowGeometry;
    windowGeometry.setX((screenGeometry.right()/100)*m_osdGeometry.x());
    windowGeometry.setY((screenGeometry.bottom()/100)*m_osdGeometry.y());
    windowGeometry.setWidth((screenGeometry.width()/100)*m_osdGeometry.width());
    windowGeometry.setHeight((screenGeometry.height()/100)*m_osdGeometry.height());

    setGeometry(windowGeometry);

}


void OSD::screenGeometryChanged(const int &screen)
{

    if (screen == qApp->desktop()->screenNumber(this)) {
        setOSDGeometry(m_osdGeometry);
    }

}


void OSD::backgroundChanged()
{

    m_validBackground = m_background->isValid();

    setAttribute(Qt::WA_TranslucentBackground, m_validBackground);
    if (m_validBackground) {
        qreal left, top, right, bottom;
        m_background->getMargins(left, top, right, bottom);
        setContentsMargins(left, top, right, bottom);

        setFont(Plasma::Theme::defaultTheme()->font(Plasma::Theme::DefaultFont));
        QPalette p = palette();
        p.setColor(QPalette::WindowText, Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor));
        setPalette(p);
    } else {
        setContentsMargins(4, 4, 4, 4);

        setFont(KGlobalSettings::generalFont());
        setPalette(qApp->palette());
    }
    updateBlur();

}


void OSD::updateBlur()
{

#if KDE_VERSION_MINOR >= 5
    const bool enableBlur = m_validBackground &&
                            Plasma::WindowEffects::isEffectAvailable(Plasma::WindowEffects::BlurBehind) &&
                            m_blurEnabled;
    Plasma::WindowEffects::enableBlurBehind(winId(), enableBlur, m_background->mask());
#endif

}


void OSD::resizeEvent(QResizeEvent *event)
{

    QWidget::resizeEvent(event);
    if (m_validBackground) {
        m_background->resizeFrame(event->size());
        setMask(m_background->mask());
    }
    updateBlur();

}


void OSD::paintEvent(QPaintEvent *event)
{

    if (m_validBackground) {
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
        painter.setClipRegion(event->region());

        m_background->paintFrame(&painter);
    } else {
        QFrame::paintEvent(event);
    }

}


} // namespace RecordItNow


#include "osd.moc"
