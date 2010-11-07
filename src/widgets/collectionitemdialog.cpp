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
#include "collectionitemdialog.h"
#include "collectionitemdialog_p.h"
#include "collection/collectionitem.h"
#include "collection/collection.h"

// KDE
#include <klocalizedstring.h>
#include <kdebug.h>


namespace RecordItNow {


CollectionItemDialogPrivate::CollectionItemDialogPrivate(CollectionItemDialog *parent)
    : QObject(parent),
    ui(new Ui::CollectionItemDialog),
    q(parent)
{

    QWidget *widget = new QWidget(q);
    ui->setupUi(widget);
    q->setMainWidget(widget);

    connect(q, SIGNAL(finished(int)), this, SLOT(dialogFinished(int)));

}


CollectionItemDialogPrivate::~CollectionItemDialogPrivate()
{


}


void CollectionItemDialogPrivate::init(RecordItNow::CollectionItem *item)
{

    itemPtr = item;
    if (itemPtr) {
        q->setCaption(i18n("Edit: %1", item->title()));

        ui->titleEdit->setText(item->title());
        ui->commentEdit->setText(item->comment());
        ui->authorEdit->setText(item->author());
        ui->ratingEdit->setRating(item->rating());
        ui->dateEdit->setDateTime(item->date());
        ui->urlEdit->setUrl(item->url());

        if (!item->isCollection()) {
            ui->iconButton->hide();
            ui->iconLabel->hide();
        } else {
            ui->iconButton->setIcon(static_cast<Collection*>(item)->icon());
        }
    }

}


void CollectionItemDialogPrivate::dialogFinished(const int &ret)
{

    if (itemPtr && ret == KDialog::Accepted) {
        CollectionItem *item = itemPtr.data();

        item->setTitle(ui->titleEdit->text());
        item->setComment(ui->commentEdit->toPlainText());
        item->setAuthor(ui->authorEdit->text());
        item->setRating(ui->ratingEdit->rating());
        item->setDate(ui->dateEdit->dateTime());
        item->setUrl(ui->urlEdit->url());

        if (item->isCollection()) {
            static_cast<Collection*>(item)->setIcon(ui->iconButton->icon());
        }
    }

}



CollectionItemDialog::CollectionItemDialog(QWidget *parent)
    : KDialog(parent),
    d(new CollectionItemDialogPrivate(this))
{

    setAttribute(Qt::WA_DeleteOnClose, true);

}


CollectionItemDialog::~CollectionItemDialog()
{

    delete d;

}


void CollectionItemDialog::setItem(RecordItNow::CollectionItem *item)
{

    d->init(item);

}


} // namespace RecordItNow


#include "collectionitemdialog_p.moc"
#include "collectionitemdialog.moc"


