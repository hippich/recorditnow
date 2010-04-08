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

#ifndef IMAGEPLAYER_H
#define IMAGEPLAYER_H

// own
#include "abstractplayer.h"

// Qt
#include <QtGui/QWidget>


namespace RecordItNow {


class ImageFrame;
class ImagePlayer : public RecordItNow::AbstractPlayer
{
    Q_OBJECT


public:
    explicit ImagePlayer(QWidget *parent = 0);
    ~ImagePlayer();

    bool canPlay(const QString &mime);
    void play(const QString &file);


private:
    ImageFrame *m_frame;


};


} // namespace RecordItNow


#endif // IMAGEPLAYER_H
