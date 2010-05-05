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
#include "volumeslider.h"

// KDE
#include <kicon.h>
#include <kdebug.h>

// Phonon
#include <Phonon/AudioOutput>


namespace RecordItNow {

    
VolumeSlider::VolumeSlider(QWidget *parent) 
    : QWidget(parent)
{

    setupUi(this);

    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
    connect(muteButton, SIGNAL(clicked()), this, SLOT(muteClicked()));

}


VolumeSlider::~VolumeSlider()
{


}


void VolumeSlider::setAudioOutput(Phonon::AudioOutput *output)
{

    m_audio = output;

    connect(m_audio, SIGNAL(mutedChanged(bool)), this, SLOT(mutedChanged(bool)));
    connect(m_audio, SIGNAL(volumeChanged(qreal)), this, SLOT(volumeChanged(qreal)));

    updateMuteIcon();
    volumeChanged(m_audio->volume());

}


void VolumeSlider::updateMuteIcon()
{

    if (!m_audio) {
        return;
    }

    if (!m_audio->isMuted()) {
        KIcon icon;

        const int value = slider->value();
        const int max = slider->maximum();

        if (value > max/2) {
            icon = KIcon("audio-volume-high");
        } else if (value > max/3) {
            icon = KIcon("audio-volume-medium");
        } else {
            icon = KIcon("audio-volume-low");
        }
        muteButton->setIcon(icon);
    } else {
        muteButton->setIcon(KIcon("audio-volume-muted"));
    }

}


void VolumeSlider::mutedChanged(const bool &muted)
{

    Q_UNUSED(muted);
    updateMuteIcon();

}


void VolumeSlider::volumeChanged(const qreal &newVolume)
{

    if (slider->value() != newVolume) {
        slider->setValue(qRound(100*newVolume));
        updateMuteIcon();
    }

}


void VolumeSlider::valueChanged(const int &value)
{

    if (m_audio && m_audio->volume() != value) {
        m_audio->setVolume((static_cast<qreal>(value))*0.01);
    }

}


void VolumeSlider::muteClicked()
{

    if (m_audio) {
        m_audio->setMuted(!m_audio->isMuted());
    }

}


} // namespace RecordItNow


#include "volumeslider.moc"
