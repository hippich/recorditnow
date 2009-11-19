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


#ifndef VIDEO_H
#define VIDEO_H


// own
#include "service.h"


namespace KYouBlip {


class KDE_EXPORT Video : public KYouBlip::Service
{
    Q_OBJECT


public:
    Video(QObject *parent = 0);
    ~Video();

    QString title() const;
    QString description() const;
    QStringList keywords() const;
    KUrl url() const;
    QString category() const;
    int duration() const;
    QString author() const;
    KUrl thumbnailUrl() const;
    QDateTime published() const;
    QString file() const;
    QString thumbnail() const;
    double rating() const;
    QString license() const;

    virtual QStringList licenses() const = 0;
    virtual QStringList categorys() const = 0;


    void setTitle(const QString &title);
    void setDescription(const QString &description);
    void setKeywords(const QString &keywords);
    void setUrl(const KUrl &url);
    void setCategory(const QString &category);
    void setDuration(const int &duration);
    void setAuthor(const QString &author);
    void setThumbnailUrl(const KUrl &url);
    void setPublished(const QDateTime &date);
    void setFile(const QString &file);
    void setThumbnail(const QString &file);
    void setRating(const double &rating);
    void setLicense(const QString &license);

    void updateThumbnail(const QString &thumbnailDir);
    bool loadThumbnail(const QString &thumbnailDir);


private:
    QHash<QString, QVariant> m_data;
    KJob *m_thumbnailJob;

    QByteArray getMD5String() const;


protected slots:
    void jobFinished(KJob *job, const QByteArray &data);


signals:
    void thumbnailUpdated(const QString &thumbnail);
    void thumbnailUpdateFailed();


};


}; // namespace KYouBlip


#endif // VIDEO_H
