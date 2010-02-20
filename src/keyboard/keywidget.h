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

#ifndef KEYWIDGET_H
#define KEYWIDGET_H


// Qt
#include <QtGui/QWidget>


class QTimer;
class KeyWidget : public QWidget
{
    Q_OBJECT


public:
    explicit KeyWidget(const QString &image, QWidget *parent = 0);
    ~KeyWidget();

    void setPressed(const bool &pressed);


private:
    QPixmap m_image;
    bool m_pressed;
    QTimer *m_timer;


private slots:
    void timeout();


protected:
    void paintEvent(QPaintEvent *event);


};


#endif // KEYWIDGET_H