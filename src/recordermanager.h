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

#ifndef RECORDERMANAGER_H
#define RECORDERMANAGER_H


// own
#include "abstractrecorder.h"

// KDE
#include <kicon.h>

// Qt
#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QPointer>


typedef QPair<QString, KIcon> RecorderData;
class RecordItNowPluginManager;
class RecorderManager : public QObject
{
    Q_OBJECT


public:
    RecorderManager(QObject *parent, RecordItNowPluginManager *manager);
    ~RecorderManager();

    QList<RecorderData> getRecorder() const;
    QHash<AbstractRecorder::Feature, bool> getFeatures(const QString &recorder, QString &defaultFile);

    void startRecord(const QString &recorder, const AbstractRecorder::Data &data);
    void pauseOrContinue();
    void stop();


private:
    RecordItNowPluginManager *m_manager;
    QPointer<AbstractRecorder> m_recorder;

    void clean();


private slots:
    void recorderError(const QString &error);
    void recorderFinished(const AbstractRecorder::ExitStatus &status);


signals:
    void status(const QString &status);
    void finished(const QString &error, const bool &isVideo = false);
    void fileChanged(const QString &newFile);


};


#endif // RECORDERMANAGER_H