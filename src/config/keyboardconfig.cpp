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
#include "keyboardconfig.h"
#include "keyboardwizard.h"
#include "devicesearchdialog.h"
#include "keyboardkeyiconpage.h"
#include "helper.h"

// KDE
#include <kicon.h>
#include <kmessagebox.h>
#include <kconfiggroup.h>
#include <kstandarddirs.h>
#include <kdebug.h>

// Qt
#include <QtGui/QListWidget>
#include <QtCore/QDir>
#include <QtGui/QKeyEvent>

// C
#include <linux/input.h>


KeyboardConfig::KeyboardConfig(KConfig *cfg, QWidget *parent)
    : RecordItNow::ConfigPage(cfg, parent)
{

    setupUi(this);

    addButton->setIcon(KIcon("list-add"));
    removeButton->setIcon(KIcon("list-remove"));
    upButton->setIcon(KIcon("go-up"));
    downButton->setIcon(KIcon("go-down"));
    searchButton->setIcon(KIcon("system-search"));

    connect(addButton, SIGNAL(clicked()), this, SLOT(add()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));
    connect(upButton, SIGNAL(clicked()), this, SLOT(up()));
    connect(downButton, SIGNAL(clicked()), this, SLOT(down()));
    connect(listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
    connect(searchButton, SIGNAL(clicked()), this, SLOT(showSearchDialog()));
    connect(kcfg_keyboardDevice, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

    itemSelectionChanged();

}


QList<KeyboardKey> KeyboardConfig::readConfig(KConfig *cfg)
{

    KConfigGroup group(cfg, "Keyboard");
    return RecordItNow::Helper::listFromArray<KeyboardKey>(group.readEntry("Keys", QByteArray()));

}


void KeyboardConfig::saveConfig(const QList<KeyboardKey> &keys, KConfig *cfg)
{

    KConfigGroup group(cfg, "Keyboard");
    group.writeEntry("Keys", RecordItNow::Helper::listToArray(keys));

}


QList<KeyboardKey> KeyboardConfig::defaultKeys()
{

    QList<KeyboardKey> keys;
    QString themeDir;

    foreach (const QString dir, KeyboardKeyIconPage::themeDirs()) {
        if (QDir(dir+QDir::separator()+"default").exists()) {
            themeDir = dir+QDir::separator()+"default/";
            break;
        }
    }

    if (themeDir.isEmpty()) {
        return keys;
    }

    QKeyEvent event(QEvent::KeyPress, Qt::Key_Control, 0, QString(), false, 1);
    KeyboardKey key = KeyboardKey::eventToKey(&event);
    key.setIcon(themeDir+"ctrl.png");
    key.setCode(KEY_LEFTCTRL);

    keys.append(key);

    event = QKeyEvent(QEvent::KeyPress, Qt::Key_Shift, 0, QString(), false, 1);
    key = KeyboardKey::eventToKey(&event);
    key.setIcon(themeDir+"shift.png");
    key.setCode(KEY_LEFTSHIFT);

    keys.append(key);

    event = QKeyEvent(QEvent::KeyPress, Qt::Key_Meta, 0, QString(), false, 1);
    key = KeyboardKey::eventToKey(&event);
    key.setIcon(themeDir+"symbol-7.png");
    key.setCode(KEY_LEFTMETA);

    keys.append(key);

    return keys;

}


void KeyboardConfig::loadConfig()
{

    setKeys(readConfig(config()));

}


void KeyboardConfig::saveConfig()
{

    saveConfig(currentKeys(), config());

}


void KeyboardConfig::setDefaults()
{

    setKeys(defaultKeys());

}


QList<KeyboardKey> KeyboardConfig::currentKeys() const
{

    QList<KeyboardKey> keys;
    for (int i = 0; i < listWidget->count(); i++) {
        QListWidgetItem *item = listWidget->item(i);

        KeyboardKey key(item->data(Qt::UserRole+1).toInt(),
                        item->data(Qt::UserRole+2).toString(),
                        item->text());
        keys.append(key);
    }
    return keys;

}


void KeyboardConfig::setKeys(const QList<KeyboardKey> &keys)
{

    listWidget->clear();
    foreach (const KeyboardKey &key, keys) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(key.text());
        item->setIcon(KIcon(key.icon()));
        item->setData(Qt::UserRole+1, key.code());
        item->setData(Qt::UserRole+2, key.icon());

        listWidget->addItem(item);
    }

    setConfigChanged(readConfig(config()) != currentKeys());

}


void KeyboardConfig::add()
{

    KeyboardWizard *wizard = new KeyboardWizard(kcfg_keyboardDevice->text(), this);
    connect(wizard, SIGNAL(wizardFinished(int, QString, QString)), this,
            SLOT(wizardFinished(int, QString, QString)));

    wizard->show();

}


void KeyboardConfig::remove()
{

    QList<QListWidgetItem*> items = listWidget->selectedItems();
    if (items.isEmpty()) {
        return;
    }

    QListWidgetItem *item = items.first();

    listWidget->takeItem(listWidget->row(item));
    delete item;

    setConfigChanged(readConfig(config()) != currentKeys());

}


void KeyboardConfig::up()
{

    QList<QListWidgetItem*> items = listWidget->selectedItems();
    if (items.isEmpty()) {
        return;
    }

    QListWidgetItem *item = items.first();
    const int index = listWidget->row(item);

    listWidget->takeItem(index);
    listWidget->insertItem(index-1, item);

    listWidget->setCurrentItem(item);

    setConfigChanged(readConfig(config()) != currentKeys());

}


void KeyboardConfig::down()
{

    QList<QListWidgetItem*> items = listWidget->selectedItems();
    if (items.isEmpty()) {
        return;
    }

    QListWidgetItem *item = items.first();
    const int index = listWidget->row(item);

    listWidget->takeItem(index);
    listWidget->insertItem(index+1, item);

    listWidget->setCurrentItem(item);

    setConfigChanged(readConfig(config()) != currentKeys());

}


void KeyboardConfig::itemSelectionChanged()
{

    if(listWidget->selectedItems().isEmpty()) {
        upButton->setEnabled(false);
        downButton->setEnabled(false);
        removeButton->setEnabled(false);
    } else {
        const int index = listWidget->row(listWidget->selectedItems().first());
        upButton->setEnabled(index != 0);
        downButton->setEnabled(index != listWidget->count()-1);
        removeButton->setEnabled(true);
    }

}


void KeyboardConfig::wizardFinished(const int &key, const QString &icon, const QString &text)
{

    for (int i = 0; i < listWidget->count(); i++) {
        if (listWidget->item(i)->data(Qt::UserRole+1).toInt() == key) {
            KMessageBox::error(this, i18n("There is already a key with code '%1' in your list!", key));
            return;
        }

    }

    QListWidgetItem *item = new QListWidgetItem();
    item->setText(text);
    item->setIcon(KIcon(icon));
    item->setData(Qt::UserRole+1, key);
    item->setData(Qt::UserRole+2, icon);

    listWidget->addItem(item);

    setConfigChanged(readConfig(config()) != currentKeys());

}


void KeyboardConfig::showSearchDialog()
{

    DeviceSearchDialog *dialog = new DeviceSearchDialog(KeyMon::DeviceInfo::KeyboardType, this);
    connect(dialog, SIGNAL(deviceSelected(QString)), this, SLOT(searchDialogFinished(QString)));

    dialog->show();

}


void KeyboardConfig::searchDialogFinished(const QString &uuid)
{

    kcfg_keyboardDevice->setText(uuid);

}


void KeyboardConfig::textChanged(const QString &text)
{

    addButton->setDisabled(text.isEmpty());

}


#include "keyboardconfig.moc"

