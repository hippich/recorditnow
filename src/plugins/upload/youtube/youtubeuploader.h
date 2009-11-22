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

#ifndef YOUTUBEUPLOADER_H
#define YOUTUBEUPLOADER_H

// own
#include "abstractuploader.h"
#include "ui_youtube.h"

// Qt
#include <QtCore/QVariantList>
#include <QtCore/QPointer>


class YouTubeService;
class YouTubeVideo;
class YouTubeUploader : public AbstractUploader, public Ui::YouTube
{
    Q_OBJECT


public:
    YouTubeUploader(QObject *parent = 0, const QVariantList &args = QVariantList());
    ~YouTubeUploader();

    void show(const QString &file, QWidget *parent);
    void cancel();


private:
    QPointer<QWidget> m_dialog;
    YouTubeService *m_service;


private slots:
    void upload();
    void cancelUpload();
    void uploadFinished(YouTubeVideo *video, const QString &id);
    void uploadCanceled(const QString &id);
    void quitDialog();
    void authenticated(const QString &account);
    void serviceError(const QString &reason, const QString &id);
    void descriptionChanged();
    void addAccount();
    void removeAccount();
    void editAccount();
    void accountsChanged(const QStringList &accounts);
    void currentAccountChanged(const QString &newAccount);
    void gotPasswordForAccount(const QString &account, const QString &password);


protected:
    bool eventFilter(QObject *watched, QEvent *event);


};


#endif // YOUTUBEUPLOADER_H
