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

#ifndef SIZEWIDGET_H
#define SIZEWIDGET_H


// own
#include "ui_sizewidget.h"

// Qt
#include <QtGui/QWidget>


class SizeWidget : public QWidget, public Ui::SizeWidget
{
    Q_OBJECT


public:
    explicit SizeWidget(QWidget *parent = 0);

    QSize getSize() const;

    void setSize(const QSize &size);


private slots:
    void valueChanged(const int &value);


protected:
    void resizeEvent(QResizeEvent *event);


signals:
    void valueChanged();
    void sizeChanged();


};


#endif // SIZEWIDGET_H
