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
class YouTubeUploader : public AbstractUploader, public Ui::YouTube
{
    Q_OBJECT


public:
    YouTubeUploader(QObject *parent = 0, const QVariantList &args = QVariantList());
    ~YouTubeUploader();

    void show(const QString &file, QWidget *parent);


private:
    enum State { Idle = 0, Upload = 1 };

    State m_state;
    QHash<QString, QString> m_category;
    QPointer<QWidget> m_dialog;
    YouTubeService *m_service;

    void setState(const State &state);


private slots:
    void upload();
    void cancelUpload();
    void uploadFinished();
    void quitDialog();
    void authenticated();
    void serviceError(const QString &error);
    void descriptionChanged();
    void addAccount();
    void removeAccount();
    void editAccount();
    void accountsChanged(const QStringList &accounts);
    void currentAccountChanged(const QString &newAccount);
    void gotPasswordForAccount(const QString &account, const QString &password);


};


#endif // YOUTUBEUPLOADER_H
