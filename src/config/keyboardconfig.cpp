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
#include "keyboardkeyiconpage.h"
#include "helper.h"
#include "keyboardrow.h"
#include "listlayout.h"
#include "keymonmanager.h"

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

    keyboardConfigLayout->addWidget(KeyMonManager::self()->keylogger()->getKeyboardConfigWidget(this, cfg));

    m_layout = new RecordItNow::ListLayout(0, true);
    keyboardWidgetList->setLayout(m_layout);
    
    addButton->setIcon(KIcon("list-add"));

    connect(addButton, SIGNAL(clicked()), this, SLOT(add()));
    connect(m_layout, SIGNAL(layoutChanged()), this, SLOT(changed()));
    connect(m_layout, SIGNAL(removeRequested(QWidget*)), this, SLOT(remove(QWidget*)));

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
    KeyMonManager::self()->keylogger()->saveConfig(config());

}


void KeyboardConfig::setDefaults()
{

    setKeys(defaultKeys());

}


QList<KeyboardKey> KeyboardConfig::currentKeys() const
{

    QList<KeyboardKey> keys;
    foreach (QWidget *widget, m_layout->rows()) {
        KeyboardRow *row = static_cast<KeyboardRow*>(widget);
  
        keys.append(KeyboardKey(row->code(), row->icon(), row->text()));
    }
    return keys;

}


void KeyboardConfig::setKeys(const QList<KeyboardKey> &keys)
{

    m_layout->clear();
    foreach (const KeyboardKey &key, keys) {
        KeyboardRow *row = new KeyboardRow(this);
        connect(row, SIGNAL(changed()), this, SLOT(changed()));
        
        row->setIcon(key.icon());
        row->setText(key.text());
        row->setCode(key.code());
    
        m_layout->addRow(row);
    }

    setConfigChanged(readConfig(config()) != currentKeys());

}


void KeyboardConfig::add()
{

    KeyboardWizard *wizard = new KeyboardWizard(this);
    connect(wizard, SIGNAL(wizardFinished(int, QString, QString)), this,
            SLOT(wizardFinished(int, QString, QString)));

    wizard->show();

}


void KeyboardConfig::remove(QWidget *widget)
{

    m_layout->removeRow(widget);

    setConfigChanged(readConfig(config()) != currentKeys());

}


void KeyboardConfig::wizardFinished(const int &key, const QString &icon, const QString &text)
{

    foreach (QWidget *widget, m_layout->rows()) {
        KeyboardRow *row = static_cast<KeyboardRow*>(widget);
        
        if (row->code() == key) {
            KMessageBox::error(this, i18n("There is already a key with code '%1' in your list!", key));
            return;
        }
    }

    KeyboardRow *row = new KeyboardRow(this);
    connect(row, SIGNAL(changed()), this, SLOT(changed()));

    row->setIcon(icon);
    row->setText(text);
    row->setCode(key);

    m_layout->addRow(row);
    
    setConfigChanged(readConfig(config()) != currentKeys());

}


void KeyboardConfig::changed()
{
    
    setConfigChanged(readConfig(config()) != currentKeys());
    
}


#include "keyboardconfig.moc"

