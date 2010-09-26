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
#include "kipipage.h"
#include "src/kipi/kipiinterface.h"

// KDE
#include <kaction.h>
#include <klocale.h>
#include <kicon.h>
#include <kdebug.h>

// Qt
#include <QtGui/QListWidgetItem>
#include <QtCore/QMetaType>


namespace RecordItNow {


KIPIPage::KIPIPage(KIPI::Interface *interface, QWidget *parent)
    : QWizardPage(parent),
    m_interface(static_cast<RecordItNow::KIPIInterface*>(interface))
{

    setupUi(this);

    connect(listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(currentSelectionChanged()));

    setPlugin(0);

}


KIPIPage::~KIPIPage()
{

    if (m_plugin && wizard()->result() == QWizard::Accepted) {
        m_plugin->trigger();
    }

}


bool KIPIPage::isComplete() const
{

    return m_plugin ? true : false;

}


void KIPIPage::setPlugin(KAction *plugin)
{

    m_plugin = plugin;
    emit completeChanged();

}


void KIPIPage::initializePage()
{

    listWidget->clear();

    QList<KAction*> plugins = m_interface->exportPlugins();
    for (int i = 0; i < plugins.size(); i++) {
        const KAction *action = plugins.at(i);

        QListWidgetItem *item = new QListWidgetItem;
        item->setText(KGlobal::locale()->removeAcceleratorMarker(action->text()));
        item->setIcon(KIcon(action->icon()));
        item->setData(Qt::UserRole+1, i);

        listWidget->addItem(item);
    }

}


void KIPIPage::currentSelectionChanged()
{

    QList<QListWidgetItem*> items = listWidget->selectedItems();
    if (items.isEmpty()) {
        setPlugin(0);
        return;
    }

    KAction *plugin = m_interface->exportPlugins().at(items.value(0)->data(Qt::UserRole+1).toInt());
    setPlugin(plugin);

}



} // namespace RecordItNow


#include "kipipage.moc"
