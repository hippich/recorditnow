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

#ifndef ABSTRACTPLAYER_H
#define ABSTRACTPLAYER_H


// Qt
#include <QtGui/QWidget>


namespace RecordItNow {


class AbstractPlayer : public QWidget
{
    Q_OBJECT


public:
    explicit AbstractPlayer(QWidget *parent = 0);
    virtual ~AbstractPlayer();

    virtual QString name() = 0;
    virtual bool canPlay(const QString &mime) = 0;
    virtual void play(const QString &file) = 0;
    virtual void stop() {};


};


} // namespace RecordItNow


#endif // ABSTRACTPLAYER_H
