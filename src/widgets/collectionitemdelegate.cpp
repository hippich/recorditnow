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
#include "collectionitemdelegate.h"
#include "collectionitemdelegate_p.h"
#include "collection/collectionitem.h"
#include "collectionlistwidgetitem.h"

// KDE
#include <klocalizedstring.h>
#include <kdebug.h>
#include <kicon.h>
#include <kglobal.h>
#include <klocale.h>
#include <nepomuk/kratingpainter.h>
#include <klineedit.h>
#include <kiconloader.h>
#include <ksvgrenderer.h>
#include <kfilterdev.h>

// Qt
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionViewItem>
#include <QtCore/QModelIndex>
#include <QtGui/QApplication>
#include <QtGui/QTextDocument>
#include <QtGui/QStyle>
#include <QtGui/QToolButton>
#include <QtCore/QFile>
#include <QtCore/QBuffer>


namespace RecordItNow {



#define SPACING 8
#define TEXT_SPACING 2
#define MARGINS 5
CollectionItemDelegatePrivate::CollectionItemDelegatePrivate(CollectionItemDelegate *parent, QAbstractItemView *v, const bool &iList)
    : QObject(parent),
    q(parent)
{

    itemList = iList;
    spacing = 4;
    leftMargin = topMargin = rightMargin = bottomMargin = 5;

    view = v;

    if (iList) {
        QPalette palette = view->palette();
        QColor color = palette.color(QPalette::Base);
        color.setAlpha(0);
        palette.setColor(QPalette::Base, color);
        color = palette.color(QPalette::AlternateBase);
        color.setAlpha(77);
        palette.setColor(QPalette::AlternateBase, color);
        view->setPalette(palette);
    }

}


CollectionItemDelegatePrivate::~CollectionItemDelegatePrivate()
{


}


QStyle *CollectionItemDelegatePrivate::style() const
{

    return view->style();

}


QString CollectionItemDelegatePrivate::getText(const int &role, const QModelIndex *index) const
{

    QString text;
    switch (role) {
    case CollectionItem::DateRole: return i18n("Date: %1", KGlobal::locale()->formatDateTime(index->data(role).toDateTime(), KLocale::ShortDate));
    case CollectionItem::ThumbnailRole: return QString::null;
    case CollectionItem::RatingRole: return QString::null;
    case CollectionItem::AuthorRole: return i18n("Author: %1", index->data(role).toString());
    case CollectionItem::UrlRole: return i18n("Location: %1", index->data(role).value<KUrl>().pathOrUrl());
    default: return index->data(role).toString();
    }

}


QFont CollectionItemDelegatePrivate::titleFont(const QStyleOptionViewItem *option) const
{

    QFont font = textFont(option);
    font.setBold(true);
    font.setUnderline(true);
    font.setPointSize(font.pointSize()+1);
    return font;

}


QFont CollectionItemDelegatePrivate::textFont(const QStyleOptionViewItem *option) const
{

    return option->font;

}


QHash<QString, QRect> CollectionItemDelegatePrivate::getLayout(const QModelIndex *index,
                                                                const QStyleOptionViewItem *option,
                                                                const bool &minimum) const
{

    const QFontMetrics titleFM(titleFont(option));
    const QFontMetrics textFM(textFont(option));

    const int textX = option->rect.left()+leftMargin+option->decorationSize.width()+q->spacing();
    int maxTextWidth = 200;
    if (!minimum) {
        maxTextWidth = option->rect.width()-leftMargin-textX;
    }

    QRect thumbnailRect(QPoint(0, 0), option->decorationSize);
    QRect titleRect(0, 0, qMin(maxTextWidth, titleFM.width(getText(CollectionItem::TitleRole, index))), titleFM.height());
    QRect commentRect(0, 0, qMin(maxTextWidth, textFM.width(getText(CollectionItem::CommentRole, index))), textFM.height()*2);
    QRect authorRect(0, 0, qMin(maxTextWidth, textFM.width(getText(CollectionItem::AuthorRole, index))), textFM.height());
    QRect ratingRect(0, 0, qMin(maxTextWidth, KIconLoader::SizeSmall*5), KIconLoader::SizeSmall);
    QRect dateRect(0, 0, textFM.width(getText(CollectionItem::DateRole, index)), textFM.height());
    QRect urlRect(0, 0, qMin(maxTextWidth, textFM.width(getText(CollectionItem::UrlRole, index))), textFM.height());

    thumbnailRect.moveLeft(option->rect.left()+leftMargin);
    thumbnailRect.moveTop(option->rect.top()+topMargin);

    titleRect.moveLeft(textX);
    titleRect.moveTop(thumbnailRect.top());

    commentRect.moveLeft(textX);
    commentRect.moveTop(titleRect.bottom()+q->spacing());

    ratingRect.moveLeft(textX);
    ratingRect.moveTop(commentRect.bottom()+q->spacing());

    authorRect.moveLeft(textX);
    authorRect.moveTop(ratingRect.bottom()+q->spacing());

    dateRect.moveLeft(textX);
    dateRect.moveTop(authorRect.bottom()+q->spacing());

    urlRect.moveLeft(textX);
    urlRect.moveTop(dateRect.bottom()+q->spacing());

    QRect cRect(option->rect.left()+leftMargin,
                option->rect.top()+topMargin,
                thumbnailRect.width()+q->spacing()+maxTextWidth,
                qMax(thumbnailRect.height(), titleRect.united(urlRect).height()));

    thumbnailRect.moveCenter(QPoint(thumbnailRect.center().x(), cRect.center().y()));

    QRect rect(cRect.adjusted(-leftMargin, -topMargin, rightMargin, bottomMargin));

    QHash<QString, QRect> rects;
    rects.insert("Rect", rect);
    rects.insert("ContentsRect", cRect);
    rects.insert("ThumbnailRect", thumbnailRect);
    rects.insert("TitleRect", titleRect);
    rects.insert("CommentRect", commentRect);
    rects.insert("AuthorRect", authorRect);
    rects.insert("DateRect", dateRect);
    rects.insert("RatingRect", ratingRect);
    rects.insert("UrlRect", urlRect);

    return rects;

}


QString CollectionItemDelegatePrivate::displayText(const QRect &boundingRect, QPainter *painter,
                                                   const QTextOption &options, const QString &text) const
{

    QString newText = text;
    const int chop = 1;
    while (!boundingRect.contains(painter->boundingRect(boundingRect, newText, options).toRect())) {
        newText.chop(chop);

        if (newText.isEmpty()) {
            break;
        }
    }
    if (newText != text) {
        newText.chop(3);
        newText.append("...");
    }
    return newText;

}


QPalette::ColorGroup CollectionItemDelegatePrivate::colorGroup(const QStyle::State &state) const
{

    if (state & QStyle::State_Enabled) {
        if (state & QStyle::State_Active) {
            return QPalette::Active;
        } else {
            return QPalette::Inactive;
        }
    } else {
        return QPalette::Disabled;
    }

}


void CollectionItemDelegatePrivate::drawText(QPainter *painter, const QString &text, const QStyleOptionViewItem *option,
                                             const QRect &rect, const bool &title) const
{

    painter->save();

    QTextOption textOptions;
    textOptions.setAlignment(Qt::AlignLeft);
    textOptions.setWrapMode(title ? QTextOption::NoWrap : QTextOption::WordWrap);

    painter->setFont(title ? titleFont(option) : textFont(option));

    const QPalette::ColorGroup group = colorGroup(option->state);
    if (option->state & QStyle::State_Selected /*|| option->state & QStyle::State_MouseOver*/) {
        painter->setPen(option->palette.color(group, QPalette::HighlightedText));
    } else {
        painter->setPen(option->palette.color(group, QPalette::Text));
    }

    painter->drawText(rect, displayText(rect, painter, textOptions, text), textOptions);

    painter->restore();

}


void CollectionItemDelegatePrivate::drawBackground(QPainter *painter, const QStyleOptionViewItem *option,
                                                   const QModelIndex *index, const QRect &rect,
                                                   const QRect &contentsRect) const
{

    painter->save();


    style()->drawPrimitive(QStyle::PE_PanelItemViewItem, option, painter);

    QStyleOptionFrameV3 opt;
    opt.state = option->state;
    opt.frameShape = QFrame::HLine;
    opt.lineWidth = 1;
    opt.midLineWidth = 0;
    opt.state |= QStyle::State_Sunken;
/*
    if (index->row() == 0) {
        opt.rect = QRect(rect.topLeft(), QSize(rect.width(), 2));
        style()->drawControl(QStyle::CE_ShapedFrame, &opt, painter, 0);
    }
*/
    opt.rect = QRect(rect.bottomLeft()-QPoint(0, 1), QSize(rect.width(), 2));
    style()->drawControl(QStyle::CE_ShapedFrame, &opt, painter, 0);


    painter->restore();

}


void CollectionItemDelegatePrivate::drawThumbnail(QPainter *painter, const QStyleOptionViewItem *option,
                                                  const QModelIndex *index, const QRect &rect) const

{

    painter->save();

    QPixmap pixmap;
    if (index->data(CollectionListWidgetItem::ThumbnailBusyRole).toBool()) {
        const int frame = index->data(CollectionListWidgetItem::ThumbnailBusyFrameRole).toInt();
        if (frame < 0 || frame >= 8) {
            kDebug() << "invalid frame:" << frame;
        } else {
            const QBrush fillBrush(option->palette.color(colorGroup(option->state),
                                                         option->state & QStyle::State_Selected ?
                                                         QPalette::HighlightedText : QPalette::WindowText),
                                   Qt::SolidPattern);
            const QBrush emptyBrush = QBrush(Qt::NoBrush);


            pixmap = QPixmap(KIconLoader::SizeLarge, KIconLoader::SizeLarge);
            pixmap.fill(Qt::transparent);

            QPainter painter(&pixmap);
            painter.setRenderHints(QPainter::Antialiasing);
            painter.setPen(option->palette.color(colorGroup(option->state),
                                                 option->state & QStyle::State_Selected ?
                                                 QPalette::HighlightedText : QPalette::WindowText));

            const QRect rect = pixmap.rect().adjusted(1, 1, -1, -1);
            const QRect e(0, 0, pixmap.width()/6, pixmap.height()/6);

            QRect top = e;
            top.moveCenter(rect.center());
            top.moveTop(rect.top());

            QRect right = e;
            right.moveCenter(rect.center());
            right.moveRight(rect.right());

            QRect bottom = e;
            bottom.moveCenter(rect.center());
            bottom.moveBottom(rect.bottom());

            QRect left = e;
            left.moveCenter(rect.center());
            left.moveLeft(rect.left());

            QRect topRight = e;
            topRight.moveCenter(top.united(right).center());

            QRect bottomRight = e;
            bottomRight.moveCenter(right.united(bottom).center());

            QRect bottomLeft = e;
            bottomLeft.moveCenter(bottom.united(left).center());

            QRect topLeft = e;
            topLeft.moveCenter(left.united(top).center());

            const QList<QRect> rects = QList<QRect>() << top << topRight << right <<
                                       bottomRight << bottom << bottomLeft << left <<
                                       topLeft;
            for (int i = 0; i < rects.size(); i++) {
                if (frame >= i) {
                    painter.setBrush(fillBrush);
                } else {
                    painter.setBrush(emptyBrush);
                }
                painter.drawEllipse(rects.at(i));
            }
        }
    }

    if (pixmap.isNull() && index->data(CollectionItem::ThumbnailRole).canConvert<QPixmap>()) {
       pixmap = index->data(CollectionItem::ThumbnailRole).value<QPixmap>();
    }

    if (pixmap.isNull()) {
        pixmap = KIcon("image-x-generic").pixmap(KIconLoader::SizeMedium);
    }

    if (pixmap.isNull()) {
        kDebug() << "NULL thumbnail pixmap!";
        return;
    }

    if (pixmap.width() > rect.width() || pixmap.height() > rect.height()) {
        pixmap = pixmap.scaled(rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    QRect pixmapRect = pixmap.rect();
    pixmapRect.moveCenter(rect.center());

    painter->drawPixmap(pixmapRect.topLeft(), pixmap);

    painter->restore();

}


CollectionItemDelegate::CollectionItemDelegate(QAbstractItemView *parent, const bool &itemList)
    : QStyledItemDelegate(parent),
    d(new CollectionItemDelegatePrivate(this, parent, itemList))
{

    qRegisterMetaTypeStreamOperators<KUrl>("KUrl");

}


CollectionItemDelegate::~CollectionItemDelegate()
{

    delete d;

}


int CollectionItemDelegate::spacing() const
{

    return d->spacing;

}


void CollectionItemDelegate::setContentsMargins(const int &left, const int &top, const int &right, const int &bottom)
{

    d->leftMargin = qMax(0, left);
    d->topMargin = qMax(0, top);
    d->rightMargin = qMax(0, right);
    d->bottomMargin = qMax(0, bottom);

}


void CollectionItemDelegate::getContentsMargins(int *left, int *top, int *right, int *bottom) const
{

    *left = d->leftMargin;
    *top = d->topMargin;
    *right = d->rightMargin;
    *bottom = d->bottomMargin;

}


void CollectionItemDelegate::setSpacing(const int &spacing)
{

    d->spacing = qMax(0, spacing);

}


void CollectionItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    //QTime time;
    //time.start();

    if (!d->itemList) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    painter->save();

    painter->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform);
    painter->setClipRect(option.rect);
    painter->setFont(option.font);

    QHash<QString, QRect> rects = d->getLayout(&index, &option, false);

    // background
    d->drawBackground(painter, &option, &index, rects.value("Rect"), rects.value("ContentsRect"));
    // thumbnail
    d->drawThumbnail(painter, &option, &index, rects.value("ThumbnailRect"));
    // Title
    d->drawText(painter, d->getText(CollectionItem::TitleRole, &index), &option, rects.value("TitleRect"), true);
    // comment
    d->drawText(painter, d->getText(CollectionItem::CommentRole, &index), &option, rects.value("CommentRect"));
    // rating
    KRatingPainter::paintRating(painter, rects.value("RatingRect"), Qt::AlignLeft, index.data(CollectionItem::RatingRole).toInt(), -1);
    // author
    d->drawText(painter, d->getText(CollectionItem::AuthorRole, &index), &option, rects.value("AuthorRect"));
    // date
    d->drawText(painter, d->getText(CollectionItem::DateRole, &index), &option, rects.value("DateRect"));
    // url
    d->drawText(painter, d->getText(CollectionItem::UrlRole, &index), &option, rects.value("UrlRect"));

    painter->restore();

    //kDebug() << "Paint:" << time.elapsed();

}


QSize CollectionItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    if (d->itemList) {
        QStyleOptionViewItem copy = option;
        copy.rect = QRect();
        QSize hint =  d->getLayout(&index, &copy, true).value("Rect").size();
        return hint;
    } else {
        return QStyledItemDelegate::sizeHint(option, index);
    }

}


} // namespace RecordItNow


#include "collectionitemdelegate_p.moc"
#include "collectionitemdelegate.moc"

