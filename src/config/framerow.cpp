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


// own
#include "framerow.h"

// KDE
#include <KIntNumInput>
#include <klineedit.h>

// Qt
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>


FrameRow::FrameRow(QWidget *parent)
    : QWidget(parent)
{

    QHBoxLayout *layout = new QHBoxLayout;
    
    m_nameEdit = new KLineEdit(this);
    m_widthInput = new KIntNumInput(this);
    m_heightInput = new KIntNumInput(this);
    QLabel *xLabel = new QLabel(this);
    
    xLabel->setText("x");
    xLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    
    layout->addWidget(m_nameEdit);
    layout->addWidget(m_widthInput);
    layout->addWidget(xLabel);
    layout->addWidget(m_heightInput);
    setLayout(layout);
    
    connect(m_nameEdit, SIGNAL(textChanged(QString)), this, SIGNAL(changed()));
    connect(m_widthInput, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(m_heightInput, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));

}
    

QString FrameRow::name() const
{
    
    return m_nameEdit->text();
    
}


QSize FrameRow::size() const
{
    
    return QSize(m_widthInput->value(), m_heightInput->value());

}
    

void FrameRow::setName(const QString &name)
{

    m_nameEdit->setText(name);
    
}


void FrameRow::setSize(const QSize &size)
{

    m_widthInput->setValue(size.width());
    m_heightInput->setValue(size.height());
    
}
    
    
    
#include "framerow.moc"
    