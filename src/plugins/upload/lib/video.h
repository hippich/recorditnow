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

    /**
     * @return the title
     */
    QString title() const;
    /**
     * @return the description
     */
    QString description() const;
    /**
     * @return the keywords/tags
     */
    QStringList keywords() const;
    /**
     * @return the url to the video
     */
    KUrl url() const;
    /**
     * @return the translated category
     */
    virtual QString category() const = 0;
    /**
     * @return the duration in seconds
     */
    int duration() const;
    /**
     * @return the author
     */
    QString author() const;
    /**
     * @return the thumbnail url
     */
    KUrl thumbnailUrl() const;
    /**
     * @return the date and time when the video was published
     */
    QDateTime published() const;
    /**
     * @return the local video file
     */
    QString file() const;
    /**
     * @return the local thumbnail
     */
    QString thumbnail() const;
    /**
     * @return the rating
     */
    double rating() const;
    /**
     * @return the translated license name
     */
    virtual QString license() const = 0;

    /**
     * @return all available licenses
     */
    virtual QStringList licenses() const = 0;
    /**
     * @return all available categorys (translated)
     */
    virtual QStringList categorys() const = 0;

    /**
     * sets the title for the video
     *
     * @param title the title
     */
    void setTitle(const QString &title);
    /**
     * sets the description for the video
     *
     * @param description the description
     */
    void setDescription(const QString &description);
    /**
     * sets the tags/keywords for the video
     *
     * @param keywords comma-separated list of tags
     */
    void setKeywords(const QString &keywords);
    /**
     * sets the url for the video
     *
     * @param url the url
     */
    void setUrl(const KUrl &url);
    /**
     * sets the category for the video
     *
     * @param category the category (translated or untranslated)
     */
    virtual void setCategory(const QString &category) = 0;
    /**
     * sets the duration for the video
     *
     * @param duration the duration in seconds
     */
    void setDuration(const int &duration);
    /**
     * sets the author of this video
     *
     * @param author the author
     */
    void setAuthor(const QString &author);
    /**
     * sets the thumbnail url for the video
     *
     * @param url the url
     */
    void setThumbnailUrl(const KUrl &url);
    /**
     * sets the date and time at which the video was published
     *
     * @param date the date/time
     */
    void setPublished(const QDateTime &date);
    /**
     * sets the local video file
     *
     * @param file the local file
     */
    void setFile(const QString &file);
    /**
     * sets the local thumbnail for the video
     *
     * @param file the thumbnail
     */
    void setThumbnail(const QString &file);
    /**
     * sets the rating for the video
     *
     * max: 5.0
     * min: 1.0
     *
     * @param rating the rating
     */
    void setRating(const double &rating);
    /**
     * sets the license for the video
     *
     * @param the license
     */
    virtual void setLicense(const QString &license) = 0;

    /**
     * updates the thumbnail for the video
     *
     * @param thumbnailDir the folder where the thumbnail should be saved
     * @see loadThumbnail()
     */
    void updateThumbnail(const QString &thumbnailDir);
    /**
     * trying to load the thumbnail from a local folder
     *
     * @param thumbnailDir the folder where the thumbnail was saved
     * @return false if the thumbnail does not exist
     * @return true if the thumbnail was found
     * @see updateThumbnail()
     */
    bool loadThumbnail(const QString &thumbnailDir);


private:
    QHash<QString, QVariant> m_data;
    KJob *m_thumbnailJob;

    QByteArray getMD5String() const;


protected slots:
    void jobFinished(KJob *job, const QByteArray &data);


signals:
    /**
     * The thumbnail was successfully updated
     */
    void thumbnailUpdated(const QString &thumbnail);
    /**
     * The update failed
     */
    void thumbnailUpdateFailed();


};


}; // namespace KYouBlip


#endif // VIDEO_H
