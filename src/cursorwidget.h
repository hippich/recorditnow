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


// Qt
#include <QtGui/QWidget>
#include <QtCore/QThread>
#include <QtGui/QColor>
#include <QtCore/QHash>


class QTimer;
class CursorWidget : public QWidget
{
    Q_OBJECT


public:
    CursorWidget(QWidget *parent);
    ~CursorWidget();

    void setSize(const QSize &size);
    void setNormalColor(const QColor &color);
    void setButtons(const QHash<int, QColor> &buttons);

    void start();
    void click(const int &button);
    WId getWindow() const;


private:
    QTimer *m_timer;
    QTimer *m_resetTimer;
    QColor m_normalColor;
    QColor m_currentColor;
    QHash<int, QColor> m_buttons;


private slots:
    void updatePos();
    void resetColor();
    void updateGrab(const bool &grab);


protected:
    void paintEvent(QPaintEvent *event);


signals:
    void error(const QString &message);


};


#endif // CURSORWIDGET_H
