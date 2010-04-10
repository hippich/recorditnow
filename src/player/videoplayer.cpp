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
#include "videoplayer.h"

// KDE
#include <kdebug.h>
#include <klocalizedstring.h>

// Phonon
#include <Phonon/MediaSource>
#include <Phonon/MediaObject>


namespace RecordItNow {


VideoPlayer::VideoPlayer(QWidget *parent)
    : RecordItNow::AbstractPlayer(parent)
{

    setupUi(this);
    playPauseButton->setIcon(KIcon("media-playback-start"));

    connect(player->mediaObject(), SIGNAL(stateChanged(Phonon::State, Phonon::State)), this,
            SLOT(stateChanged(Phonon::State,Phonon::State)));

    connect(playPauseButton, SIGNAL(clicked()), this, SLOT(playPauseClicked()));

    player->mediaObject()->setTickInterval(1000);
    seekSlider->setMediaObject(player->mediaObject());
    volumeSlider->setAudioOutput(player->audioOutput());

}


VideoPlayer::~VideoPlayer()
{



}


QString VideoPlayer::name()
{

    return i18n("Video Player");

}


bool VideoPlayer::canPlay(const QString &mime)
{

    return mime.startsWith(QLatin1String("video/"));

}


void VideoPlayer::play(const QString &file)
{

    player->play(Phonon::MediaSource(file));

}


void VideoPlayer::stop()
{

    player->stop();

}


void VideoPlayer::playPauseClicked()
{

    if (player->isPaused()) {
        player->play();
    } else {
        player->pause();
    }

}


void VideoPlayer::stateChanged(const Phonon::State &newState, const Phonon::State &oldState)
{

    Q_UNUSED(oldState);

    switch (newState) {
    case Phonon::PlayingState: playPauseButton->setIcon(KIcon("media-playback-pause")); break;
    case Phonon::StoppedState:
    case Phonon::PausedState: playPauseButton->setIcon(KIcon("media-playback-start")); break;
    default: break;
    }

}



} // namespace RecordItNow


#include "videoplayer.moc"
