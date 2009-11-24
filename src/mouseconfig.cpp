/***************************************************************************
 *   Copyright (C) 2009 by Kai Dombrowe <just89@gmx.de>                    *
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
#include "mouseconfig.h"
#include <recorditnow.h>

// KDE
#include <kicon.h>
#include <kcolorbutton.h>

// Qt
#include <QtGui/QTreeWidget>


MouseConfig::MouseConfig(QWidget *parent)
    : QWidget(parent)
{

    setupUi(this);

    addButton->setIcon(KIcon("list-add"));
    removeButton->setIcon(KIcon("list-remove"));

    connect(addButton, SIGNAL(clicked()), this, SLOT(addClicked()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeClicked()));
    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));

}


MouseConfig::~MouseConfig()
{




}


void MouseConfig::saveConfig()
{

    KConfig *cfg = Settings::self()->config();
    KConfigGroup cfgGroup(cfg, "Mouse");

    int buttons = 0;
    for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);
        const int button = item->text(0).toInt();
        const QColor color = static_cast<KColorButton*>(treeWidget->itemWidget(item, 1))->color();

        cfgGroup.writeEntry(QString("Button %1 key").arg(QString::number(buttons)), button);
        cfgGroup.writeEntry(QString("Button %1 color").arg(QString::number(buttons)), color);

        buttons++;
    }
    cfgGroup.writeEntry("Buttons", buttons);

}


void MouseConfig::loadConfig()
{

    QHash<int, QColor> buttons = getButtons();
    QHashIterator<int, QColor> it(buttons);
    while (it.hasNext()) {
        it.next();

        QTreeWidgetItem *item = new QTreeWidgetItem();
        KColorButton *button = newButton();

        item->setText(0, QString::number(it.key()));
        button->setColor(it.value());

        treeWidget->addTopLevelItem(item);
        treeWidget->setItemWidget(item, 1, button);

    }

    if (buttons.isEmpty()) {
        defaults();
    }

}


void MouseConfig::defaults()
{

    treeWidget->clear();

    KColorButton *button1 = newButton();
    KColorButton *button3 = newButton();

    button1->setColor(Qt::red);
    button3->setColor(Qt::yellow);

    QTreeWidgetItem *item1 = new QTreeWidgetItem();
    QTreeWidgetItem *item3 = new QTreeWidgetItem();

    item1->setText(0, QString::number(1));
    item3->setText(0, QString::number(3));

    // left click
    treeWidget->addTopLevelItem(item1);
    treeWidget->setItemWidget(item1, 1, button1);

    // right click
    treeWidget->addTopLevelItem(item3);
    treeWidget->setItemWidget(item3, 1, button3);

    emit configChanged();

}


QHash<int, QColor> MouseConfig::getButtons()
{

    QHash<int, QColor> buttons;

    KConfig *cfg = Settings::self()->config();
    KConfigGroup cfgGroup(cfg, "Mouse");

    int keys = cfgGroup.readEntry("Buttons", 0);
    for (int i = 0; i < keys; i++) {
        const QString button = QString::number(i);
        const int key = cfgGroup.readEntry(QString("Button %1 key").arg(button), 0);
        const QColor color = cfgGroup.readEntry(QString("Button %1 color").arg(button), QColor());

        buttons[key] = color;
    }

    return buttons;

}


KColorButton *MouseConfig::newButton()
{

    KColorButton *button = new KColorButton(this);
    connect(button, SIGNAL(changed(QColor)), this, SIGNAL(configChanged()));
    return button;

}


void MouseConfig::addClicked()
{

    if (addEdit->text().isEmpty()) { // text?
        return;
    }

    bool ok;
    addEdit->text().toInt(&ok);
    if (!ok) { // int?
        return;
    }

    for (int i = 0; i < treeWidget->topLevelItemCount(); i++) { // double?
        if (treeWidget->topLevelItem(i)->text(0) == addEdit->text()) {
            return;
        }
    }

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, addEdit->text());

    treeWidget->addTopLevelItem(item);
    treeWidget->setItemWidget(item, 1, newButton());

    emit configChanged();

}


void MouseConfig::removeClicked()
{

    QList<QTreeWidgetItem*> childs = treeWidget->selectedItems();

    if (childs.isEmpty()) {
        return;
    }

    QTreeWidgetItem *root = treeWidget->invisibleRootItem();
    foreach (QTreeWidgetItem *child, childs) {
        root->removeChild(child);
    }

    emit configChanged();

}


void MouseConfig::itemSelectionChanged()
{

    removeButton->setDisabled(treeWidget->selectedItems().isEmpty());

}


