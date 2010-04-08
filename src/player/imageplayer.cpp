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
#include "imageplayer.h"
#include "imageframe.h"

// KDE
#include <kdebug.h>
#include <klocalizedstring.h>

// Qt
#include <QtGui/QHBoxLayout>


namespace RecordItNow {


ImagePlayer::ImagePlayer(QWidget *parent)
    : RecordItNow::AbstractPlayer(parent)
{

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);

    m_frame = new ImageFrame(this);
    layout->addWidget(m_frame);

    setLayout(layout);

}


ImagePlayer::~ImagePlayer()
{



}


QString ImagePlayer::name()
{

    return i18n("Image Viewer");

}


bool ImagePlayer::canPlay(const QString &mime)
{

    return mime.startsWith(QLatin1String("image/"));

}


void ImagePlayer::play(const QString &file)
{

    m_frame->setPixmap(QPixmap(file));

}


} // namespace RecordItNow


#include "imageplayer.moc"
