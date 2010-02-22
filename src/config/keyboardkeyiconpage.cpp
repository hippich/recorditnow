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
#include "keyboardkeyiconpage.h"

// KDE
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kicon.h>

// Qt
#include <QtCore/QDir>
#include <QtGui/QListWidgetItem>


KeyboardKeyIconPage::KeyboardKeyIconPage(QWidget *parent)
    : QWizardPage(parent)
{

    setupUi(this);
    searchLine->setListWidget(iconList);

    registerField("Icon*", iconEdit, "text", SIGNAL(textChanged(QString)));

    const QString iconDirPath = KGlobal::dirs()->locate("data", "recorditnow/icons/");
    QDir dir(iconDirPath);

    foreach (const QString &file, dir.entryList(QStringList(), QDir::Files)) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setIcon(KIcon(dir.absoluteFilePath(file)));
        item->setText(file);

        iconList->addItem(item);
    }

    m_iconDir = iconDirPath;


    connect(iconList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(currentItemChanged(QListWidgetItem*,QListWidgetItem*)));

}


void KeyboardKeyIconPage::currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{

    Q_UNUSED(previous);

    if (!current) {
        iconEdit->clear();
    } else {
        iconEdit->setText(m_iconDir+current->text());
    }

}




#include "keyboardkeyiconpage.moc"

