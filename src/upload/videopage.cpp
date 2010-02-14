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
#include "videopage.h"
#include "uploadwizard.h"

// JoschyCore
#include <joschycore/manager.h>
#include <joschycore/plugininfo.h>
#include <joschycore/abstractprovider.h>

// KDE
#include <kmessagebox.h>

// Qt
#include <QtCore/QFile>


VideoPage::VideoPage(QWidget *parent)
    : QWizardPage(parent)
{

    setupUi(this);

    registerField("Video*", videoRequester, "text", SIGNAL(textChanged(QString)));
    registerField("Title*", titleEdit);
    registerField("Description*", descriptionEdit, "plainText", SIGNAL(textChanged()));
    registerField("Category*", categoryCombo, "currentText", SIGNAL(currentIndexChanged(QString)));
    registerField("Tags*", tagsEdit);

}


void VideoPage::initializePage()
{

    const Joschy::PluginInfo provider = static_cast<UploadWizard*>(wizard())->provider();
    Joschy::AbstractProvider *plugin = Joschy::Manager::self()->createProvider(provider.name(), "QNetworkLayer");

    if (!plugin) {
        KMessageBox::error(this, i18n("Cannot load: %1", provider.name()));
        return;
    }

    categoryCombo->addItems(plugin->categorys());

    Joschy::Manager::self()->unloadProvider(plugin);

}


#include "videopage.moc"
