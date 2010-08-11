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


// own
#include "encoderscriptadaptor.h"
#include "script.h"

// Qt
#include <QtScript/QScriptEngine>
#include <QtCore/QTimer>


namespace RecordItNow {


EncoderScriptAdaptor::EncoderScriptAdaptor(RecordItNow::Script *script, QObject *parent)
    : RecordItNow::AbstractEncoder(parent),
    m_script(script)
{



}


EncoderScriptAdaptor::~EncoderScriptAdaptor()
{

    m_script->unload();

}


bool EncoderScriptAdaptor::initPlugin()
{

    if (m_script->load()) {
        QScriptValue value = m_script->engine()->newQObject(this);
        m_script->engine()->globalObject().setProperty("RecordItNowEncoder", value);
        return true;
    }
    return false;

}


void EncoderScriptAdaptor::encode(const Data &d)
{

    m_script->engine()->globalObject().setProperty("Outputfile", d.file);
    m_script->engine()->globalObject().setProperty("Overwrite", d.overwrite);
    m_script->engine()->globalObject().setProperty("WorkDir", d.workDir);

    const QScriptValue value = m_script->engine()->evaluate("encode();");
    m_script->handleException(value);

}


void EncoderScriptAdaptor::pause()
{

    const QScriptValue value = m_script->engine()->evaluate("pause();");
    m_script->handleException(value);

}


void EncoderScriptAdaptor::stop()
{

    const QScriptValue value = m_script->engine()->evaluate("stop();");
    m_script->handleException(value);

}


void EncoderScriptAdaptor::sendStatus(const QString &text)
{

    emit status(text);

}


void EncoderScriptAdaptor::sendError(const QString &text)
{

    emit error(text);

}


void EncoderScriptAdaptor::sendNewOutputfile(const QString &newFile)
{

    emit outputFileChanged(newFile);

}


void EncoderScriptAdaptor::exit(const int &status)
{

    m_status = status;
    QTimer::singleShot(0, this, SLOT(emitFinished()));

}


void EncoderScriptAdaptor::emitFinished()
{

    emit finished((RecordItNow::AbstractEncoder::ExitStatus) m_status);

}


} // namespace RecordItNow


#include "encoderscriptadaptor.moc"
