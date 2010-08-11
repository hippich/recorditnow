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
#include "recorderscriptadaptor.h"
#include "script.h"
#include "scriptimporter.h"

// KDE
#include <kdebug.h>

// Qt
#include <QtScript/QScriptEngine>
#include <QtCore/QTimer>


namespace RecordItNow {


RecorderScriptAdaptor::RecorderScriptAdaptor(RecordItNow::Script *script, QObject *parent)
    : RecordItNow::AbstractRecorder(parent),
    m_script(script)
{



}


RecorderScriptAdaptor::~RecorderScriptAdaptor()
{

    m_script->unload();

}

// unused
int RecorderScriptAdaptor::zoomFactor() const
{

    const QScriptValue value = m_script->engine()->evaluate("zoomFactor();");
    m_script->handleException(value);

    return value.toInt32();

}


bool RecorderScriptAdaptor::isVideoRecorder() const
{

    const QScriptValue value = m_script->engine()->evaluate("isVideoRecorder();");
    m_script->handleException(value);

    return value.toBool();

}


bool RecorderScriptAdaptor::initPlugin()
{

    if (m_script->load()) {
        QScriptValue value = m_script->engine()->newQObject(this);
        m_script->engine()->globalObject().setProperty("RecordItNowRecorder", value);
        return true;
    }
    return false;

}


void RecorderScriptAdaptor::record(const AbstractRecorder::Data &data)
{

    m_script->engine()->globalObject().setProperty("FPS", data.fps);
    m_script->engine()->globalObject().setProperty("WorkDir", data.workDir);
    m_script->engine()->globalObject().setProperty("SoundEnabled", data.sound);
    m_script->engine()->globalObject().setProperty("X", data.geometry.x());
    m_script->engine()->globalObject().setProperty("Y", data.geometry.y());
    m_script->engine()->globalObject().setProperty("Width", data.geometry.width());
    m_script->engine()->globalObject().setProperty("Height", data.geometry.height());
    m_script->engine()->globalObject().setProperty("Window", (int) data.winId);
    m_script->engine()->globalObject().setProperty("Overwrite", data.overwrite);
    m_script->engine()->globalObject().setProperty("MouseMarkSize", data.mouseMarkSize);
    m_script->engine()->globalObject().setProperty("Outputfile", data.outputFile);

    const QScriptValue value = m_script->engine()->evaluate("record();");
    m_script->handleException(value);

}


void RecorderScriptAdaptor::pause()
{

    const QScriptValue value = m_script->engine()->evaluate("pause();");
    m_script->handleException(value);

}


void RecorderScriptAdaptor::stop()
{

    const QScriptValue value = m_script->engine()->evaluate("stop();");
    m_script->handleException(value);

}

// unused
void RecorderScriptAdaptor::mouseClick(const QColor &color, const bool &pressed, const int &mode)
{

    m_script->engine()->globalObject().setProperty("MouseClickColor", color.name());
    m_script->engine()->globalObject().setProperty("MouseClickPressed", pressed);
    m_script->engine()->globalObject().setProperty("MouseClickMode", mode);

    const QScriptValue value = m_script->engine()->evaluate("mouseClick();");
    m_script->handleException(value);


}

// unused
void RecorderScriptAdaptor::setZoomFactor(const int &factor)
{

    m_script->engine()->globalObject().setProperty("ZoomFactor", factor);
    emit zoomFactorChanged();

}


void RecorderScriptAdaptor::setRecorderState(const int &newState)
{

    setState((RecordItNow::AbstractRecorder::State) newState);

}


void RecorderScriptAdaptor::exit(const int &status)
{

    m_status = status;
    QTimer::singleShot(0, this, SLOT(emitFinished()));

}


void RecorderScriptAdaptor::sendStatus(const QString &message)
{

    emit status(message);

}


void RecorderScriptAdaptor::sendError(const QString &message)
{

    emit error(message);

}


void RecorderScriptAdaptor::emitFinished()
{

    emit finished((RecordItNow::AbstractRecorder::ExitStatus) m_status);

}


} // namespace RecordItNow


#include "recorderscriptadaptor.moc"
