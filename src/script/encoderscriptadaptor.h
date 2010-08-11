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

#ifndef RECORDITNOW_ENCODERSCRIPTADAPTOR_H
#define RECORDITNOW_ENCODERSCRIPTADAPTOR_H


// own
#include "libs/encoder/abstractencoder.h"


namespace RecordItNow {


class Script;
class EncoderScriptAdaptor : public RecordItNow::AbstractEncoder
{
    Q_OBJECT


public:
    explicit EncoderScriptAdaptor(RecordItNow::Script *script, QObject *parent = 0);
    ~EncoderScriptAdaptor();

    bool initPlugin();

    void encode(const Data &d);
    void pause();
    void stop();


public slots:
    void sendStatus(const QString &text);
    void sendError(const QString &text);
    void sendNewOutputfile(const QString &newFile);
    void exit(const int &status);


private:
    RecordItNow::Script *m_script;
    int m_status;

private slots:
    void emitFinished();


};


} // namespace RecordItNow


#endif // RECORDITNOW_ENCODERSCRIPTADAPTOR_H
