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


// own
#include "abstractrecorder.h"

// KDE
#include <kglobal.h>
#include <kstandarddirs.h>
#include <klocalizedstring.h>
#include <kdebug.h>

// Qt
#include <QtCore/QDir>


namespace RecordItNow {


AbstractRecorder::AbstractRecorder(QObject *parent, const QVariantList &args)
    : RecordItNow::Plugin(args, parent)
{

    m_state = Idle;
    qRegisterMetaType<RecordItNow::AbstractRecorder::ExitStatus>("RecordItNow::AbstractRecorder::ExitStatus");

}


AbstractRecorder::~AbstractRecorder()
{




}


RecordItNow::AbstractRecorder::State AbstractRecorder::state() const
{

    return m_state;

}


void AbstractRecorder::setState(const RecordItNow::AbstractRecorder::State &newState)
{

    if (m_state == newState) {
        return;
    }
    m_state = newState;
    emit stateChanged(newState);

}


} // namespace RecordItNow


#include "abstractrecorder.moc"

