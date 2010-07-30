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

#ifndef RECORDITNOW_ENCODERMANAGER_H
#define RECORDITNOW_ENCODERMANAGER_H


// own
#include "abstractencoder.h"

// KDE
#include <kicon.h>

// Qt
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QPair>


namespace RecordItNow {


typedef QPair<QString, KIcon> EncoderData;
class RecordItNowPluginManager;
class EncoderManager : public QObject
{
    Q_OBJECT


public:
    EncoderManager(QObject *parent, RecordItNowPluginManager *manager);
    ~EncoderManager();

    QList<EncoderData> getEncoder() const;

    void startEncode(const QString &recorder, const AbstractEncoder::Data &data);
    void pauseOrContinue();
    void stop();


private:
    RecordItNowPluginManager *m_manager;
    QPointer<AbstractEncoder> m_encoder;

    void clean();


private slots:
    void encoderError(const QString &error);
    void encoderFinished(const RecordItNow::AbstractEncoder::ExitStatus &status);


signals:
    void status(const QString &status);
    void finished(const QString &error);
    void fileChanged(const QString &newFile);


};


} // namespace RecordItNow


#endif // RECORDITNOW_ENCODERMANAGER_H
