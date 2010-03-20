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

#ifndef TIMERWIDGET_H
#define TIMERWIDGET_H


// own
#include "ui_timerwidget.h"

// Qt
#include <QtGui/QFrame>
#include <QtCore/QWeakPointer>


class KNotification;
class QTimer;
namespace RecordItNow {


class TimerWidget : public QFrame, Ui::TimerWidget
{
    Q_OBJECT


public:
    explicit TimerWidget(QWidget *parent = 0);
    ~TimerWidget();

    int value() const;
    void setValue(const int &value);


public slots:
    void start();
    void pause();
    void reset();


private:
    QTimer *m_timer;
    int m_time;
    QWeakPointer<KNotification> m_tickNotification;

    void stopTimerInternal(const bool &closeNotification = true);


private slots:
    void upClicked();
    void downClicked();
    void tick();


signals:
    void timeout();


};


} // namespace RecordItNow


#endif // TIMERWIDGET_H
