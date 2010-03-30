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
#include "framesize.h"


FrameSize::FrameSize(const QSize &size, const QString &text)
    : m_size(size), m_text(text)
{


}


FrameSize::FrameSize(const FrameSize &other)
    : m_size(other.size()), m_text(other.text())
{


}


FrameSize::FrameSize()
    : m_size(QSize()), m_text(QString())
{



}


FrameSize::~FrameSize()
{


}


QSize FrameSize::size() const
{

    return m_size;

}


QString FrameSize::text() const
{

    return m_text;

}


void FrameSize::setSize(const QSize &size)
{

    m_size = size;

}


void FrameSize::setText(const QString &text)
{

    m_text = text;

}


bool FrameSize::operator==(const FrameSize &other) const
{

    return (other.size() == size() && other.text() == text());

}


QDataStream &operator<<(QDataStream &stream, const FrameSize &data)
{

    stream << data.size() << data.text();
    return stream;

}


QDataStream &operator>>(QDataStream &stream, FrameSize &data)
{

    QSize size;
    QString text;

    stream >> size;
    stream >> text;

    data.setSize(size);
    data.setText(text);

    return stream;

}

