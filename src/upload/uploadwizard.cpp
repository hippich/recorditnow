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
#include "uploadwizard.h"
#include "pluginpage.h"
#include "videopage.h"
#include "uploadpage.h"
#include "accountpage.h"

// KDE
#include <kicon.h>
#include <kiconloader.h>


UploadWizard::UploadWizard(QWidget *parent)
    : QWizard(parent)
{

    setAttribute(Qt::WA_DeleteOnClose);
    addPage(new PluginPage(this));
    addPage(new AccountPage(this));
    addPage(new VideoPage(this));
    addPage(new UploadPage(this));

    setOption(QWizard::DisabledBackButtonOnLastPage, true);
    setPixmap(QWizard::LogoPixmap, KIcon("recorditnow-upload-media").pixmap(KIconLoader::SizeMedium,
                                                                            KIconLoader::SizeMedium));
}


Joschy::PluginInfo UploadWizard::provider() const
{

    return static_cast<PluginPage*>(page(0))->plugin();

}


#include "uploadwizard.moc"
