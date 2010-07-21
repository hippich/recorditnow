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

#ifndef RECORDITNOW_KEYMONMANAGER_H
#define RECORDITNOW_KEYMONMANAGER_H


// own
#include "libs/keylogger/abstractkeylogger.h"


class KeyMonManager : public QObject
{
    Q_OBJECT
    friend class KeyMonManagerSingleton;


public:
    ~KeyMonManager();
    static KeyMonManager *self();


    bool isRunning() const;
    QString error() const;
    bool start();
    void stop();
    bool waitForStarted();

    RecordItNow::AbstractKeylogger *keylogger() const;


private:
    static KeyMonManager *m_self;
    RecordItNow::AbstractKeylogger *m_logger;

    explicit KeyMonManager(QObject *parent = 0);


signals:
    void keyEvent(const RecordItNow::KeyloggerEvent &event);
    void stopped();
    void started();


};


#endif // RECORDITNOW_KEYMONMANAGER_H
