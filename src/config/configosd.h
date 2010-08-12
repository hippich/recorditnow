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

#ifndef RECORDITNOW_CONFIGOSD_H
#define RECORDITNOW_CONFIGOSD_H


// own
#include "widgets/osd.h"


class QToolButton;
class QSpinBox;
namespace RecordItNow {


class ConfigOSD : public RecordItNow::OSD
{
    Q_OBJECT


public:
    explicit ConfigOSD(QWidget *parent = 0);
    ~ConfigOSD();


private:
    QToolButton *m_moveButton;
    QSpinBox *m_hSpin;
    QSpinBox *m_wSpin;
    QPoint m_lastPos;

    void checkGeometry();


private slots:
    void movePressed();
    void widthValueChanged(const int &value);
    void heightValueChanged(const int &value);


protected:
    bool eventFilter(QObject *watched, QEvent *event);

    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


signals:
    void OSDChanged();


};


} // namespace RecordItNow


#endif // RECORDITNOW_CONFIGOSD_H
