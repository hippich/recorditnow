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
#include "timelinetopicsdialog.h"
#include "topicwidget.h"
#include "topic.h"

// KDE
#include <kicon.h>
#include <klocalizedstring.h>
#include <kicondialog.h>
#include <kdebug.h>

// Qt
#include <QtGui/QTimeEdit>


TimeLineTopicsDialog::TimeLineTopicsDialog(QWidget *parent, TopicWidget *topicWidget)
    : KDialog(parent), m_topicWidget(topicWidget)
{

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(i18n("Topics"));

    QWidget *widget = new QWidget(this);
    setupUi(widget);    
    setMainWidget(widget);

    addButton->setIcon(KIcon("list-add"));
    removeButton->setIcon(KIcon("list-remove"));

    connect(addButton, SIGNAL(clicked()), this, SLOT(addTopic()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeTopic()));
    connect(this, SIGNAL(finished(int)), this, SLOT(updateTopicWidget(int)));

    treeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);

    // load
    QList<Topic*> topics = m_topicWidget->topics();
    for (int i = 0; i < topics.size(); i++) {
        addTopic(); // create widgets
    }

    for (int i = 0; i < topics.size(); i++) {
        Topic *topic = topics[i];
        QTreeWidgetItem *item = treeWidget->invisibleRootItem()->child(i);

        QTimeEdit *durationEdit = static_cast<QTimeEdit*>(treeWidget->itemWidget(item, 2));
        KIconButton *button = static_cast<KIconButton*>(treeWidget->itemWidget(item, 1));

        item->setText(0, topic->title());
        button->setIcon(topic->icon());
        durationEdit->setTime(topic->duration());
    }


}


TimeLineTopicsDialog::~TimeLineTopicsDialog()
{


}


void TimeLineTopicsDialog::addTopic()
{

    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, i18n("Untitled"));
    item->setFlags(Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    KIconButton *button = new KIconButton(this);
    QTimeEdit *begin = new QTimeEdit(this);

    begin->setDisplayFormat("hh:mm:ss");
    button->setMaximumSize(KIconLoader::SizeMedium, KIconLoader::SizeMedium);
    button->setMinimumSize(KIconLoader::SizeMedium, KIconLoader::SizeMedium);
    button->setIconSize(KIconLoader::SizeSmall);

    button->setIcon("dialog-information");

    treeWidget->invisibleRootItem()->addChild(item);
    treeWidget->setItemWidget(item, 1, button);
    treeWidget->setItemWidget(item, 2, begin);

}


void TimeLineTopicsDialog::removeTopic()
{

    QList<QTreeWidgetItem*> items = treeWidget->selectedItems();
    foreach (QTreeWidgetItem *child, items) {
        treeWidget->invisibleRootItem()->removeChild(child);
    }

}


void TimeLineTopicsDialog::updateTopicWidget(const int &ret)
{

    if (ret != KDialog::Accepted) {
        return;
    }

    m_topicWidget->clear();

    for (int i = 0; i < treeWidget->invisibleRootItem()->childCount(); i++) {
        QTreeWidgetItem *item = treeWidget->invisibleRootItem()->child(i);

        QString title = item->text(0);
        QString icon = static_cast<KIconButton*>(treeWidget->itemWidget(item, 1))->icon();
        QTime dTime = static_cast<QTimeEdit*>(treeWidget->itemWidget(item, 2))->time();

        kDebug() << "icon:" << static_cast<KIconButton*>(treeWidget->itemWidget(item, 1))->icon();

        m_topicWidget->addTopic(dTime, title, icon);
    }

}

