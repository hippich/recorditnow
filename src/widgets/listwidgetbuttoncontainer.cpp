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
#include "listwidgetbuttoncontainer_p.h"

// KDE
#include <kpushbutton.h>
#include <kseparator.h>
#include <kdebug.h>
#include <klocalizedstring.h>

// Qt
#include <QtGui/QHBoxLayout>


namespace RecordItNow {


ListWidgetButtonContainer::ListWidgetButtonContainer(QListView *parent)
    : QWidget(parent)
{

}


QWidget *ListWidgetButtonContainer::button(const RecordItNow::CollectionListWidget::ButtonCode &button) const
{

    if (m_buttons.contains(button)) {
        return m_buttons.value(button);
    } else {
        return 0;
    }

}


void ListWidgetButtonContainer::setButtons(const RecordItNow::CollectionListWidget::ButtonCodes &buttons)
{

    m_buttons.clear();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(-1, -1, -1, 0);
    mainLayout->setSizeConstraint(QVBoxLayout::SetMinimumSize);

    KSeparator *mainSeparator = new KSeparator(this);
    mainSeparator->setFrameShape(KSeparator::HLine);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSizeConstraint(QLayout::SetMinimumSize);

    mainLayout->addLayout(layout);
    mainLayout->addWidget(mainSeparator);

    QToolButton *button = 0;
    if (buttons & CollectionListWidget::PlayButton) {
        button = newButton(this);
        button->setIcon(KIcon("media-playback-start"));
        button->setToolTip(i18n("Play"));
        connect(button, SIGNAL(clicked()), this, SIGNAL(playClicked()));

        layout->addWidget(button);
        m_buttons.insert(CollectionListWidget::PlayButton, button);
    }
/*
    if (button) {
        KSeparator *separator = new KSeparator(this);
        separator->setFrameShape(QFrame::VLine);
        layout->addWidget(separator);
    }
*/

    if (buttons & CollectionListWidget::UploadButton) {
        button = newButton(this);
        button->setIcon(KIcon("recorditnow-upload-media"));
        button->setToolTip(i18n("Upload"));
        connect(button, SIGNAL(clicked()), this, SIGNAL(uploadClicked()));

        layout->addWidget(button);
        m_buttons.insert(CollectionListWidget::UploadButton, button);
    }

    if (buttons & CollectionListWidget::AddButton) {
        button = newButton(this);
        button->setIcon(KIcon("list-add"));
        button->setToolTip(i18n("Add"));
        connect(button, SIGNAL(clicked()), this, SIGNAL(addClicked()));

        layout->addWidget(button);
        m_buttons.insert(CollectionListWidget::AddButton, button);
    }

    if (buttons & CollectionListWidget::EditButton) {
        button = newButton(this);
        button->setIcon(KIcon("document-edit"));
        button->setToolTip(i18n("Edit"));
        connect(button, SIGNAL(clicked()), this, SIGNAL(editClicked()));

        layout->addWidget(button);
        m_buttons.insert(CollectionListWidget::EditButton, button);
    }

    if (buttons & CollectionListWidget::DeleteButton) {
        button = newButton(this);
        button->setIcon(KIcon("edit-delete"));
        button->setToolTip(i18n("Delete"));
        connect(button, SIGNAL(clicked()), this, SIGNAL(deleteClicked()));

        layout->addWidget(button);
        m_buttons.insert(CollectionListWidget::DeleteButton, button);
    }

    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(spacer);

    setLayout(mainLayout);

}


void ListWidgetButtonContainer::reLayout()
{

    QListView *list = static_cast<QListView*>(parentWidget());
    Q_ASSERT(list);
    Q_ASSERT(list->viewport());

    QRect geometry;
    geometry.setHeight(list->viewport()->geometry().top()-list->contentsRect().top());
    geometry.setWidth(list->contentsRect().width());
    geometry.moveBottomLeft(list->viewport()->geometry().topLeft());

    setGeometry(geometry);

}


QToolButton *ListWidgetButtonContainer::newButton(QWidget *parent) const
{

    QToolButton *button = new QToolButton(parent);
    button->setDisabled(true);
    button->setAutoRaise(true);

    return button;

}



void ListWidgetButtonContainer::resizeEvent(QResizeEvent *event)
{

    QWidget::resizeEvent(event);
    emit sizeChanged();

}


} // namespace RecordItNow


#include "listwidgetbuttoncontainer_p.moc"

