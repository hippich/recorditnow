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
#include "playerdock.h"
#include "imageplayer.h"
#include "videoplayer.h"

// KDE
#include <kmimetype.h>
#include <kdebug.h>

// Qt
#include <QtGui/QStackedLayout>


namespace RecordItNow {


PlayerDock::PlayerDock(QWidget *parent)
    : QDockWidget(parent)
{

    setupUi(this);

    QStackedLayout *layout = new QStackedLayout;

    AbstractPlayer *imagePlayer = new ImagePlayer(this);
    AbstractPlayer *videoPlayer = new VideoPlayer(this);

    layout->addWidget(imagePlayer);
    layout->addWidget(videoPlayer);

    contentWidget->setLayout(layout);

    m_playerWidgets.append(imagePlayer);
    m_playerWidgets.append(videoPlayer);

}


PlayerDock::~PlayerDock()
{



}


bool PlayerDock::play(const QString &file)
{

    AbstractPlayer *playerWidget = 0;

    KMimeType::Ptr mimePtr = KMimeType::findByPath(file, 0, true);
    const QString mimeType = mimePtr->name();

    foreach (AbstractPlayer *player, m_playerWidgets) {
        player->stop();
        if (player->canPlay(mimeType)) {
            playerWidget = player;
        }
    }

    if (!playerWidget) {
        return false;
    } else {
        playerWidget->play(file);
        QStackedLayout *layout = static_cast<QStackedLayout*>(contentWidget->layout());
        layout->setCurrentWidget(playerWidget);
    }

    return true;

}


} // namespace RecordItNow


#include "playerdock.moc"
