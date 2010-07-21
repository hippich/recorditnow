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

#ifndef RECORDITNOW_RECORDKEYLOGGER_H
#define RECORDITNOW_RECORDKEYLOGGER_H

// own
#include "src/libs/keylogger/abstractkeylogger.h"
#include "src/libs/keylogger/keyloggerevent.h"

// KDE
#include <kdemacros.h>

// Qt
#include <QtCore/QObject>


namespace RecordItNow {


class RecordKeyloggerPrivate;
class KDE_EXPORT RecordKeylogger: public RecordItNow::AbstractKeylogger
{
    Q_OBJECT


public:
    explicit RecordKeylogger(QObject *parent = 0);
    ~RecordKeylogger();

    bool start(const KConfig *);
    void stop();
    bool waitForStarted();
    bool isRunning();


private:
    RecordKeyloggerPrivate *d;


};


} // namespace RecordItNow


#endif // RECORDITNOW_RECORDKEYLOGGER_H
