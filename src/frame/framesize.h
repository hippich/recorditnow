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

#ifndef FRAMESIZE_H
#define FRAMESIZE_H


// Qt
#include <QtCore/QString>
#include <QtCore/QSize>
#include <QtCore/QMetaType>


class FrameSize
{


public:
    FrameSize(const QSize &size, const QString &text);
    FrameSize(const FrameSize &other);
    FrameSize();
    ~FrameSize();

    QSize size() const;
    QString text() const;

    void setSize(const QSize &size);
    void setText(const QString &text);

    bool operator==(const FrameSize &other) const;


private:
    QSize m_size;
    QString m_text;


};

Q_DECLARE_METATYPE(FrameSize)


QDataStream &operator<<(QDataStream &stream, const FrameSize &data);
QDataStream &operator>>(QDataStream &stream, FrameSize &data);



#endif // FRAMESIZE_H
