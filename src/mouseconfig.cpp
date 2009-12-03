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
#include "mousebutton.h"

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

    connect(addButton, SIGNAL(clicked()), this, SLOT(addClicked()));

    buttonCombo->addItem(MouseButton::getName(MouseButton::LeftButton));
    buttonCombo->addItem(MouseButton::getName(MouseButton::RightButton));
    buttonCombo->addItem(MouseButton::getName(MouseButton::MiddleButton));
    buttonCombo->addItem(MouseButton::getName(MouseButton::WheelUp));
    buttonCombo->addItem(MouseButton::getName(MouseButton::WheelDown));
    buttonCombo->addItem(MouseButton::getName(MouseButton::SpecialButton1));
    buttonCombo->addItem(MouseButton::getName(MouseButton::SpecialButton2));

    treeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);

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
        const int button =  static_cast<MouseButton*>(treeWidget->itemWidget(item, 1))->getXButton();
        const QColor color = static_cast<KColorButton*>(treeWidget->itemWidget(item, 2))->color();

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

        MouseButton *mouseButton = newMouseButton();
        mouseButton->setXButton(it.key());

        treeWidget->addTopLevelItem(item);
        treeWidget->setItemWidget(item, 0, newRemoveButton());
        treeWidget->setItemWidget(item, 1, mouseButton);
        treeWidget->setItemWidget(item, 2, button);

    }

    if (buttons.isEmpty()) {
        defaults();
    }

}


void MouseConfig::defaults()
{

    treeWidget->clear();

    MouseButton *mouseButton1 = newMouseButton();
    MouseButton *mouseButton3 = newMouseButton();
    MouseButton *mouseButton4 = newMouseButton();
    MouseButton *mouseButton5 = newMouseButton();
    MouseButton *mouseButton8 = newMouseButton();
    MouseButton *mouseButton9 = newMouseButton();

    mouseButton1->setButton(MouseButton::LeftButton);
    mouseButton3->setButton(MouseButton::RightButton);
    mouseButton4->setButton(MouseButton::WheelUp);
    mouseButton5->setButton(MouseButton::WheelDown);
    mouseButton8->setButton(MouseButton::SpecialButton1);
    mouseButton9->setButton(MouseButton::SpecialButton2);

    KColorButton *button1 = newButton();
    KColorButton *button3 = newButton();
    KColorButton *button4 = newButton();
    KColorButton *button5 = newButton();
    KColorButton *button8 = newButton();
    KColorButton *button9 = newButton();

    button1->setColor(Qt::red);
    button3->setColor(Qt::yellow);
    button4->setColor(Qt::darkBlue);
    button5->setColor(Qt::blue);
    button8->setColor(Qt::magenta);
    button9->setColor(Qt::darkMagenta);

    QTreeWidgetItem *item1 = new QTreeWidgetItem();
    QTreeWidgetItem *item3 = new QTreeWidgetItem();
    QTreeWidgetItem *item4 = new QTreeWidgetItem();
    QTreeWidgetItem *item5 = new QTreeWidgetItem();
    QTreeWidgetItem *item8 = new QTreeWidgetItem();
    QTreeWidgetItem *item9 = new QTreeWidgetItem();

    item1->setText(0, QString::number(1));
    item3->setText(0, QString::number(3));
    item4->setText(0, QString::number(4));
    item5->setText(0, QString::number(5));
    item8->setText(0, QString::number(8));
    item9->setText(0, QString::number(9));

    // left click
    treeWidget->addTopLevelItem(item1);
    treeWidget->setItemWidget(item1, 0, newRemoveButton());
    treeWidget->setItemWidget(item1, 1, mouseButton1);
    treeWidget->setItemWidget(item1, 2, button1);

    // right click
    treeWidget->addTopLevelItem(item3);
    treeWidget->setItemWidget(item3, 0, newRemoveButton());
    treeWidget->setItemWidget(item3, 1, mouseButton3);
    treeWidget->setItemWidget(item3, 2, button3);

    // mouse wheel
    treeWidget->addTopLevelItem(item4);
    treeWidget->setItemWidget(item4, 0, newRemoveButton());
    treeWidget->setItemWidget(item4, 1, mouseButton4);
    treeWidget->setItemWidget(item4, 2, button4);
    treeWidget->addTopLevelItem(item5);
    treeWidget->setItemWidget(item5, 0, newRemoveButton());
    treeWidget->setItemWidget(item5, 1, mouseButton5);
    treeWidget->setItemWidget(item5, 2, button5);

    // special 1
    treeWidget->addTopLevelItem(item8);
    treeWidget->setItemWidget(item8, 0, newRemoveButton());
    treeWidget->setItemWidget(item8, 1, mouseButton8);
    treeWidget->setItemWidget(item8, 2, button8);

    // special 2
    treeWidget->addTopLevelItem(item9);
    treeWidget->setItemWidget(item9, 0, newRemoveButton());
    treeWidget->setItemWidget(item9, 1, mouseButton9);
    treeWidget->setItemWidget(item9, 2, button9);


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


QToolButton *MouseConfig::newRemoveButton()
{

    QToolButton *remove = new QToolButton(this);
    remove->setMaximumWidth(KIconLoader::SizeMedium);
    remove->setIcon(KIcon("list-remove"));
    connect(remove, SIGNAL(clicked()), this, SLOT(removeClicked()));
    return remove;

}


MouseButton *MouseConfig::newMouseButton()
{

    MouseButton *button = new MouseButton(this);
    connect(button, SIGNAL(buttonChanged()), this, SIGNAL(configChanged()));
    connect(button, SIGNAL(sizeChanged()), this, SLOT(updateColumnSize()));
    return button;

}


void MouseConfig::addClicked()
{

    MouseButton::Button mButton = MouseButton::getButtonFromName(buttonCombo->currentText());
    if (mButton == MouseButton::NoButton) {
        return;
    }

    for (int i = 0; i < treeWidget->topLevelItemCount(); i++) { // double?
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);
        MouseButton *button = static_cast<MouseButton*>(treeWidget->itemWidget(item, 1));
        if (button->getMouseButton() == mButton) {
            return;
        }
    }

    MouseButton *button = newMouseButton();
    button->setButton(mButton);

    QTreeWidgetItem *item = new QTreeWidgetItem();

    treeWidget->addTopLevelItem(item);
    treeWidget->setItemWidget(item, 0, newRemoveButton());
    treeWidget->setItemWidget(item, 1, button);
    treeWidget->setItemWidget(item, 2, newButton());

    emit configChanged();

}


void MouseConfig::removeClicked()
{

    for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);
        if (sender() == treeWidget->itemWidget(item, 0)) {
            treeWidget->invisibleRootItem()->removeChild(item);
        }
    }
    emit configChanged();

}


void MouseConfig::updateColumnSize()
{

    treeWidget->header()->resizeSections(QHeaderView::ResizeToContents);

}
