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


// own
#include "sizewidget.h"

// KDE
#include <kdebug.h>


SizeWidget::SizeWidget(QWidget *parent)
    : QWidget(parent)
{

    setupUi(this);

    connect(widthInput, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
    connect(heightInput, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));

}


QSize SizeWidget::getSize() const
{

    return QSize(widthInput->value(), heightInput->value());

}


void SizeWidget::setSize(const QSize &size)
{

    widthInput->setValue(size.width());
    heightInput->setValue(size.height());

}


void SizeWidget::valueChanged(const int &value)
{

    Q_UNUSED(value);

    widthInput->setMinimumSize(widthInput->sizeHint());
    heightInput->setMinimumSize(heightInput->sizeHint());
    widthInput->setMaximumSize(widthInput->sizeHint());
    heightInput->setMaximumSize(heightInput->sizeHint());

    int width = widthInput->size().width();
    width += heightInput->sizeHint().width();
    width += label->sizeHint().width();
    width += (layout()->spacing()*3);
    width += layout()->contentsMargins().left()+layout()->contentsMargins().right();

    setMinimumSize(width, height());
    emit valueChanged();

}



void SizeWidget::resizeEvent(QResizeEvent *event)
{

    QWidget::resizeEvent(event);
    emit sizeChanged();

}


#include "sizewidget.moc"
