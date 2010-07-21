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

#ifndef RECORDITNOW_ABSTRACTKEYLOGGER_H
#define RECORDITNOW_ABSTRACTKEYLOGGER_H


// own
#include "keyloggerevent.h"

// KDE
#include <kconfiggroup.h>
#include <kdemacros.h>

// Qt
#include <QtCore/QObject>


namespace RecordItNow {


class KDE_EXPORT AbstractKeylogger : public QObject
{
    Q_OBJECT


public:
    explicit AbstractKeylogger(QObject *parent);
    ~AbstractKeylogger();

    virtual QWidget *getKeyboardConfigWidget(QWidget *parent, const KConfig *);
    virtual QWidget *getMouseConfigWidget(QWidget *parent, const KConfig *);
    virtual bool start(const KConfig *) = 0;
    virtual void stop() = 0;
    virtual bool waitForStarted() = 0;
    virtual bool isRunning() = 0;
    virtual void saveConfig(KConfig *) {};
    virtual bool hasConfigChanged(const KConfig *cfg);
    virtual QString error() const { return QString(); };


signals:
    void error(const QString &errorString);
    void started();
    void stopped();
    void keyEvent(const RecordItNow::KeyloggerEvent &event);
    void configChanged();


};


} // namespace RecordItNow


#endif // RECORDITNOW_ABSTRACTKEYLOGGER_H
