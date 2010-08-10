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

#ifndef RECORDITNOW_RECORDERSCRIPTADAPTOR_H
#define RECORDITNOW_RECORDERSCRIPTADAPTOR_H


// own
#include "abstractrecorder.h"

// KDE
#include <kplugininfo.h>


class QScriptEngine;
namespace RecordItNow {


class Script;
class RecorderScriptAdaptor : public RecordItNow::AbstractRecorder
{
    Q_OBJECT


public:
    explicit RecorderScriptAdaptor(RecordItNow::Script *script, QObject *parent = 0);
    ~RecorderScriptAdaptor();

    int zoomFactor() const;
    bool isVideoRecorder() const;
    bool initPlugin();

    void record(const AbstractRecorder::Data &data);
    void pause();
    void stop();

    void mouseClick(const QColor &color, const bool &pressed, const int &mode);
    void setZoomFactor(const int &factor);


public slots:
    void setRecorderState(const int &newState);
    void exit(const int &status);
    void sendStatus(const QString &message);
    void sendError(const QString &message);


private:
    Script *m_script;
    int m_status;


private slots:
    void emitFinished();


signals:
    void zoomFactorChanged();


};


} // namespace RecordItNow


#endif // RECORDITNOW_RECORDERSCRIPTADAPTOR_H
