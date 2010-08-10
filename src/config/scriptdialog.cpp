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
#include "scriptdialog.h"
#include "helper.h"
#include "script/scriptmanager.h"

// KDE
#include <kplugininfo.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kdebug.h>

// Qt
#include <QtGui/QListWidgetItem>


namespace RecordItNow {


ScriptDialog::ScriptDialog(QWidget *parent)
    : KDialog(parent)
{

    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle(i18nc("@title:window", "RecordItNow Script Manager"));

    QWidget *widget = new QWidget(this);
    setupUi(widget);
    setMainWidget(widget);

    installScriptButton->setIcon(KIcon("list-add"));
    uninstallScriptButton->setIcon(KIcon("list-remove"));

    connect(installScriptButton, SIGNAL(clicked()), this, SLOT(installClicked()));
    connect(uninstallScriptButton, SIGNAL(clicked()), this, SLOT(uninstallClicked()));

    updateScriptList();

}


ScriptDialog::~ScriptDialog()
{



}


void ScriptDialog::updateScriptList()
{

    scriptList->clear();
    foreach (const KPluginInfo &info, Helper::self()->scriptManager()->availableScripts()) {
        if (!info.entryPath().startsWith(Helper::self()->scriptManager()->scriptSaveLocation())) {
            continue; // show only user installed scripts
        }

        QListWidgetItem *item = new QListWidgetItem();
        item->setText(info.name());
        item->setIcon(KIcon(info.icon()));
        item->setData(Qt::UserRole+1, info.entryPath());

        scriptList->addItem(item);
    }

}


void ScriptDialog::installClicked()
{

    KFileDialog *dialog = new KFileDialog(KUrl(), QString(), this, 0);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    dialog->setMode(KFile::Directory|KFile::LocalOnly|KFile::ExistingOnly);

    connect(dialog, SIGNAL(okClicked()), this, SLOT(installScript()));

    dialog->show();

}


void ScriptDialog::uninstallClicked()
{

    QList<QListWidgetItem*> items = scriptList->selectedItems();
    if (items.isEmpty()) {
        return;
    }

    QStringList failed;
    foreach (QListWidgetItem *item, items) {
        const QString path = QFileInfo(item->data(Qt::UserRole+1).toString()).absolutePath();
        if (!Helper::self()->scriptManager()->uninstallScript(path)) {
            failed << path;
        }
    }

    if (!failed.isEmpty()) {
        KMessageBox::error(this, i18n("Failed to uninstall the following scripts:\n%1", failed.join("\n")));
    }

    Helper::self()->scriptManager()->reloadPluginList();
    updateScriptList();

}


void ScriptDialog::installScript()
{

    KFileDialog *dialog = static_cast<KFileDialog*>(sender());
    const QString dir = dialog->selectedFile();

    if (!dir.isEmpty()) {
        if (RecordItNow::Helper::self()->scriptManager()->installScript(dir)) {
            RecordItNow::Helper::self()->scriptManager()->reloadPluginList();

            updateScriptList();

            KMessageBox::information(this, i18n("The script \"%1\" has been successfully installed.", dir));
        } else {
            KMessageBox::error(this, i18n("Script installation failed!"));
        }
    }

}


} // namespace RecordItNow


#include "scriptdialog.moc"
