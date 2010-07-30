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

#ifndef RECORDITNOW_CURSORWIDGET_H
#define RECORDITNOW_CURSORWIDGET_H


// own
#include "src/libs/keylogger/keyloggerevent.h"
#include "mousebutton.h"
#include "abstractrecorder.h"

// Qt
#include <QtGui/QWidget>
#include <QtCore/QThread>
#include <QtGui/QColor>
#include <QtCore/QHash>
#include <QtCore/QVariantMap>
#include <QtCore/QPointer>


namespace KeyMon {
    class Device;
};

class QTimer;
namespace RecordItNow {


class CursorWidget : public QWidget
{
    Q_OBJECT


public:
    enum WidgetMode {
        LEDMode = 0,
        CircleMode = 1,
        TargetMode = 2
    };

    CursorWidget(QWidget *parent);
    ~CursorWidget();

    void setSize(const QSize &size);
    void setButtons(const QList<MouseButton> &buttons);
    void setDevice(const QString &deviceName);
    void setMode(const CursorWidget::WidgetMode &mode);
    void setOpacity(const qreal &opacity);
    void setShowAlways(const bool &show);
    void switchToPreviewMode();
    void setRecorder(AbstractRecorder *recorder);
    
    void start();
    void stop();


private:
    QTimer *m_timer;
    QTimer *m_resetTimer;
    QList<MouseButton> m_buttons;
    QString m_deviceName;
    bool m_grab;
    WidgetMode m_mode;
    qreal m_opacity;
    bool m_show;
    MouseButton m_currentButton;
    MouseButton m_defaultButton;
    QPointer<AbstractRecorder> m_recorder;

    MouseButton getButton(const int &code) const;


private slots:
    void updatePos();
    void resetColor();
    void buttonPressed(const RecordItNow::KeyloggerEvent &event);
    void previewColors();
    void updateMask();


protected:
    void paintEvent(QPaintEvent *event);
    void paintLED(QPainter *painter);
    void paintCircle(QPainter *painter);
    void paintTarget(QPainter *painter);
    void resizeEvent(QResizeEvent *event);


signals:
    void error(const QString &message);


};


} // namespace RecordItNow


#endif // RECORDITNOW_CURSORWIDGET_H

