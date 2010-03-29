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

#ifndef MouseButtonWidget_H
#define MouseButtonWidget_H


// KDE
#include <kpushbutton.h>


class MouseButtonWidget : public KPushButton
{
    Q_OBJECT


public:
    enum Button {
        NoButton = -1,
        LeftButton = 0,
        RightButton = 1,
        MiddleButton = 2,
        SpecialButton1 = 3,
        SpecialButton2 = 4,
        WheelUp = 5,
        WheelDown = 6
    };

    MouseButtonWidget(QWidget *parent = 0);
    ~MouseButtonWidget();

    int getXButton() const;
    MouseButtonWidget::Button getMouseButtonWidget() const;
    static QString getName(const MouseButtonWidget::Button &button);
    static MouseButtonWidget::Button getButtonFromName(const QString &name);
    static MouseButtonWidget::Button getButtonFromXButton(const int &button);

    void setXButton(const int &button);
    void setButton(const Button &button);


private:
    Button m_button;

    void setText(const QString &text);


private slots:
    void getButton();


protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *event);


signals:
    void buttonChanged(const MouseButtonWidget::Button &oldButton, const MouseButtonWidget::Button &newButton);
    void sizeChanged();


};


#endif // MouseButtonWidget_H
