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


#ifndef RECORDITNOW_COLLECTIONITEM_H
#define RECORDITNOW_COLLECTIONITEM_H


// KDE
#include <kurl.h>

// Qt
#include <QtCore/QObject>
#include <QtCore/QDateTime>


namespace RecordItNow {


class Collection;
class CollectionItemPrivate;
class CollectionItem : public QObject
{
    Q_OBJECT
    friend class CollectionItemPrivate;
    friend class Collection;
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString comment READ comment WRITE setComment)
    Q_PROPERTY(int rating READ rating WRITE setRating)
    Q_PROPERTY(QDateTime date READ date WRITE setDate)
    Q_PROPERTY(KUrl url READ url WRITE setUrl)
    Q_PROPERTY(QString author READ author WRITE setAuthor)


public:
    enum DataRole {
        TitleRole = Qt::DisplayRole,
        ThumbnailRole = Qt::UserRole+1,
        CommentRole = Qt::UserRole+2,
        RatingRole = Qt::UserRole+3,
        DateRole = Qt::UserRole+4,
        UrlRole = Qt::UserRole+5,
        AuthorRole = Qt::UserRole+6,
        IconRole = 7,
        CollectionRole = 8,

        UserRole = Qt::UserRole+100
    };

    explicit CollectionItem(RecordItNow::Collection *parent = 0);
    ~CollectionItem();

    virtual bool isCollection() const;
    RecordItNow::Collection *collection() const;
    QString title() const;
    QPixmap thumbnail(const QSize &size) const;
    QString comment() const;
    int rating() const;
    QDateTime date() const;
    KUrl url() const;
    QString author() const;

    QVariant value(const RecordItNow::CollectionItem::DataRole &role) const;
    void setValue(const RecordItNow::CollectionItem::DataRole &role, const QVariant &value);

    virtual QByteArray save() const;
    virtual void load(const QByteArray &data);

    void setTitle(const QString &title);
    void setThumbnail(const QPixmap &thumbnail, const QSize &size);
    void setComment(const QString &comment);
    void setRating(const int &rating);
    void setDate(const QDateTime &date);
    void setUrl(const KUrl &url);
    void setAuthor(const QString &author);

    void createThumbnail(const QSize &size);


private:
    CollectionItemPrivate *d;


signals:
    void changed(RecordItNow::CollectionItem *self, const RecordItNow::CollectionItem::DataRole &role);
    void thumbnailUpdateStarted();
    void thumbnailUpdateFinished();


};


} // namespace RecordItNow


#endif // RECORDITNOW_COLLECTIONITEM_H
