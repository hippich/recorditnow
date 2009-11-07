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
#include "uploadthread.h"

// KDE
#include <kdebug.h>
#include <klocalizedstring.h>
#include <kjob.h>

// Qt
#include <QtCore/QMetaType>

// libgdata
extern "C" {
#include <libgdata/gdata/gdata.h>
#include <gio/gio.h>
};


#define DEVELOPER_KEY "AI39si4PPp_RmxGSVs4cHH93rcG2e9vSRQU1vC0L3sfuy_ZHmtaAWZOdvSfBjmow3YSZfrerxjhsZGX0brUrdSLr5qvNchxeiQ" // RecordItNow
#define CLIENT_ID "ytapi-GNOME-libgdata-444fubtt-0"


class Job : public KJob {

public:
    Job() {};
    ~Job() {};

    void start() {};

    void setText(const QString &file)
    {

        qRegisterMetaType< QPair<QString,QString> >("QPair<QString,QString>");
        emit description(this, i18n("Upload to YouTube"), qMakePair(i18n("Video"), file));

    }

    void setInfo(const QString &message)
    {

        infoMessage(this, message);

    }

    void setProgress(const int &progress)
    {

        setPercent(progress);

    }

    void emitFinished()
    {

        emitResult();

    }

};



static GCancellable *cancel = NULL;
UploadThread::UploadThread(QObject *parent, const QHash<QString, QString> &data)
    : QThread(parent), m_data(data)
{

    m_job = new Job();

}


UploadThread::~UploadThread()
{

    if (cancel && G_IS_OBJECT(cancel)) {
        g_object_unref(cancel);
    }

    if (m_job) {
        delete m_job;
    }

}


KJob *UploadThread::getJob() const
{

    return m_job;

}


void UploadThread::cancelUpload()
{

    if (cancel && G_IS_OBJECT(cancel)) {
        kDebug() << "cancel!";
        g_cancellable_cancel(cancel);
        cancel = 0;
        deleteLater();
    }

}


void UploadThread::run()
{

    const QString title = m_data["Title"];
    const QString description = m_data["Description"];
    const QString tags = m_data["Tags"];
    const QString qcategory = m_data["Category"];
    const QString password = m_data["Password"];
    const QString login = m_data["Login"];
    const QString path = m_data["File"];

    m_job->start();
    m_job->setText(path);
    m_job->setProgress(10);

    g_type_init();

    // auth
    kDebug() << "auth...";
    m_job->setInfo(i18n("Authenticating..."));

    gboolean retval;
    GDataService *service;
    GError *error = NULL;
    cancel = g_cancellable_new();

    // Create a service
    service = GDATA_SERVICE (gdata_youtube_service_new (DEVELOPER_KEY, CLIENT_ID));
    m_job->setProgress(20);

    if (!service) {
        kDebug() << "!service";
        m_job->emitFinished();
        emit ytError(i18n("Unkown error"));
        return;
    }

    if (!GDATA_IS_SERVICE(service)) {
        g_object_unref(service);
        kDebug() << "!isService";
        m_job->emitFinished();
        emit ytError(i18n("Unkown error"));
        return;
    }

    // Log in
    m_job->setInfo(i18n("Log in..."));
    m_job->setProgress(30);

    retval = gdata_service_authenticate (service,
                                         login.toLatin1(),
                                         password.toLatin1(),
                                         cancel,
                                         &error);

    if (error) {
        g_object_unref(service);
        const QString msg = error->message;
        g_clear_error (&error);
        m_job->emitFinished();
        emit ytError(msg);
        return;
    }

    if (!retval) {
        g_object_unref(service);
        g_clear_error (&error);
        m_job->emitFinished();
        emit ytError(i18n("Authentication failed!"));        
        return;
    }
    g_clear_error(&error);
    // auth end

    if (g_cancellable_is_cancelled(cancel)) {
        kDebug() << "canceled...";
        m_job->emitFinished();
        emit finished();
        return;
    }

    // upload
    kDebug() << "upload";
    m_job->setInfo(i18n("Uploading..."));
    m_job->setProgress(50);


    GDataYouTubeVideo *video, *new_video;
    GDataMediaCategory *category;
    GFile *video_file;
    gchar *xml;
    error = NULL;
    video = gdata_youtube_video_new(NULL);

    gdata_youtube_video_set_title(video, title.toLatin1());
    gdata_youtube_video_set_description(video, description.toLatin1());
    category = gdata_media_category_new(qcategory.toLatin1(),
                                        "http://gdata.youtube.com/schemas/2007/categories.cat",
                                        NULL);
    gdata_youtube_video_set_category(video, category);
    g_object_unref(category);
    gdata_youtube_video_set_keywords(video, tags.toLatin1());

    // TODO Check the XML
    xml = gdata_parsable_get_xml(GDATA_PARSABLE(video));
    g_free (xml);

    video_file = g_file_new_for_path(path.toLatin1());

    m_job->setProgress(60);

    // Upload the video
    new_video = gdata_youtube_service_upload_video(GDATA_YOUTUBE_SERVICE(service),
                                                   video,
                                                   video_file,
                                                   cancel,
                                                   &error);
    if (error) {
        g_object_unref(service);
        g_object_unref(video);
        g_object_unref(new_video);
        g_object_unref(video_file);
        const QString msg = error->message;
        g_clear_error(&error);
        m_job->emitFinished();
        emit ytError(msg);
        return;
    }

    m_job->setProgress(100);

    g_clear_error(&error);
    g_object_unref(video);
    g_object_unref(new_video);
    g_object_unref(video_file);
    g_object_unref(service);

    kDebug() << "upload finished";
    m_job->emitFinished();
    emit finished();

}



