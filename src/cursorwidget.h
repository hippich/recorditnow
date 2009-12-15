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

#ifndef CURSORWIDGET_H
#define CURSORWIDGET_H


// own
#include "keymon/event.h"

// Qt
#include <QtGui/QWidget>
#include <QtCore/QThread>
#include <QtGui/QColor>
#include <QtCore/QHash>
#include <QtCore/QVariantMap>


namespace KeyMon {
    class Device;
};

class QTimer;
class CursorWidget : public QWidget
{
    Q_OBJECT


public:
    enum WidgetMode {
        LEDMode = 0,
        CircleMode = 1
    };

    CursorWidget(QWidget *parent);
    ~CursorWidget();

    void setSize(const QSize &size);
    void setNormalColor(const QColor &color);
    void setButtons(const QHash<int, QColor> &buttons);
    void setUseKeyMon(const bool &use, const QString &deviceName = QString());
    void setMode(const CursorWidget::WidgetMode &mode);
    void setOpacity(const qreal &opacity);
    void switchToPreviewMode();

    void start();
    void stop();
    void click(const int &button);
    WId getWindow() const;


private:
    QTimer *m_timer;
    QTimer *m_resetTimer;
    QColor m_normalColor;
    QColor m_currentColor;
    QHash<int, QColor> m_buttons;
    bool m_useKeyMon;
    KeyMon::Device *m_device;
    QString m_deviceName;
    bool m_grab;
    WidgetMode m_mode;
    qreal m_opacity;


private slots:
    void updatePos();
    void resetColor();
    void updateGrab(const bool &grab);
    void buttonPressed(const KeyMon::Event &event);
    void progressStep(const QVariantMap &data);
    void previewColors();


protected:
    void paintEvent(QPaintEvent *event);
    void paintLED(QPainter *painter);
    void paintCircle(QPainter *painter);


signals:
    void error(const QString &message);


};


#endif // CURSORWIDGET_H
