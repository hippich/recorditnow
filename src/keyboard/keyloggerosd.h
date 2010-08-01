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


#ifndef RECORDITNOW_KEYLOGGER_ON_SCREEN_DISPLAY_H
#define RECORDITNOW_KEYLOGGER_ON_SCREEN_DISPLAY_H


// own
#include "keyloggerevent.h"

// Qt
#include <QtGui/QFrame>



namespace Plasma {
    class FrameSvg;
};


class QPropertyAnimation;
namespace RecordItNow {


class KeyloggerLabel;
class KeyloggerOSD : public QFrame
{
    Q_OBJECT


public:
    KeyloggerOSD(QWidget *parent = 0);
    virtual ~KeyloggerOSD();

    void init(const int &timeout, const int &fontSize, const bool &shortcuts, const bool &clipboard);


private:
    Plasma::FrameSvg *m_background;
    QPropertyAnimation *m_animation;
    QTimer *m_timer;
    KeyloggerLabel *m_edit;
    QTimer *m_hideTimer;
    bool m_inactive;
    bool m_validBackground;


private slots:
    void updateMousePos();
    void screenGeometryChanged(const int &screen);
    void backgroundChanged();
    void updateGeometry();
    void inactive();
    void keyloggerEvent(const RecordItNow::KeyloggerEvent &event);
    void clipboardDataChanged();


protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);


};


} // namespace RecordItNow


#endif // RECORDITNOW_KEYLOGGER_ON_SCREEN_DISPLAY_H
