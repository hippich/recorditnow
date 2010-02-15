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

// JoschyCore
#include <joschycore/manager.h>
#include <joschycore/plugininfo.h>
#include <joschycore/abstractprovider.h>

// KDE
#include <kmessagebox.h>

// Qt
#include <QtCore/QFile>


VideoPage::VideoPage(QWidget *parent)
    : QWizardPage(parent), m_provider(0)
{

    setupUi(this);

    registerField("Video*", videoRequester, "text", SIGNAL(textChanged(QString)));
    registerField("Title*", titleEdit);
    registerField("Description*", descriptionEdit, "plainText", SIGNAL(textChanged()));
    registerField("Category*", categoryCombo, "currentText", SIGNAL(currentIndexChanged(QString)));
    registerField("Tags*", tagBox, "items", SIGNAL(changed()));

}


VideoPage::~VideoPage()
{

    if (m_provider) {
        Joschy::Manager::self()->unloadProvider(m_provider);
    }

}


void VideoPage::initializePage()
{

    if (m_provider) {
        Joschy::Manager::self()->unloadProvider(m_provider);
        m_provider = 0;
    }

    Joschy::AbstractProvider *plugin = Joschy::Manager::self()->createProvider(field("Provider").toString(),
                                                                               "QNetworkLayer");

    if (!plugin) {
        KMessageBox::error(this, i18n("Cannot load: %1", field("Provider").toString()));
        return;
    }

    m_provider = plugin;

    connect(plugin, SIGNAL(categorysChanged(QStringList)), this, SLOT(categorysChanged(QStringList)));
    categoryCombo->addItems(plugin->categorys());

}


void VideoPage::categorysChanged(const QStringList &categorys)
{

    categoryCombo->clear();
    categoryCombo->addItems(categorys);

}


#include "videopage.moc"
