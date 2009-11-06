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

// libgdata
extern "C" {
#include <libgdata/gdata/gdata.h>
#include <gio/gio.h>
};


#define DEVELOPER_KEY "AI39si4PPp_RmxGSVs4cHH93rcG2e9vSRQU1vC0L3sfuy_ZHmtaAWZOdvSfBjmow3YSZfrerxjhsZGX0brUrdSLr5qvNchxeiQ" // RecordItNow
#define CLIENT_ID "ytapi-GNOME-libgdata-444fubtt-0"


static GCancellable *cancel = NULL;
UploadThread::UploadThread(QObject *parent, const QHash<QString, QString> &data)
    : QThread(parent), m_data(data)
{



}


UploadThread::~UploadThread()
{

    if (cancel) {
        g_object_unref(cancel);
    }

}


void UploadThread::cancelUpload()
{

    if (cancel) {
        kDebug() << "cancel!!!!";
        g_cancellable_cancel(cancel);
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

    g_type_init();

    // auth
    kDebug() << "auth...";

    gboolean retval;
    GDataService *service;
    GError *error = NULL;
    cancel = g_cancellable_new();

    // Create a service
    service = GDATA_SERVICE (gdata_youtube_service_new (DEVELOPER_KEY, CLIENT_ID));

    if (!service) {
        kDebug() << "!service";
        emit ytError(i18n("Unkown error"));
        return;
    }

    if (!GDATA_IS_SERVICE(service)) {
        g_object_unref(service);
        kDebug() << "!isService";
        emit ytError(i18n("Unkown error"));
        return;
    }

    // Log in
    retval = gdata_service_authenticate (service,
                                         login.toLatin1(),
                                         password.toLatin1(),
                                         cancel,
                                         &error);

    if (error) {
        g_object_unref(service);
        const QString msg = error->message;
        g_clear_error (&error);
        emit ytError(msg);
        return;
    }

    if (!retval) {
        g_object_unref(service);
        g_clear_error (&error);
        emit ytError(i18n("Authentication failed!"));
        return;
    }
    g_clear_error(&error);
    // auth end

    if (g_cancellable_is_cancelled(cancel)) {
        kDebug() << "canceled...";
        emit finished();
        return;
    }

    // upload
    kDebug() << "upload";

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

    video_file = g_file_new_for_path(m_data["File"].toLatin1());

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
        emit ytError(msg);
        return;
    }
    g_clear_error(&error);
    g_object_unref(video);
    g_object_unref(new_video);
    g_object_unref(video_file);
    g_object_unref(service);

    kDebug() << "upload finished";
    emit finished();

}



