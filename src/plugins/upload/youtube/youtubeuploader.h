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


namespace KWallet {
    class Wallet;
};

class UploadThread;
class YouTubeUploader : public AbstractUploader, public Ui::YouTube
{
    Q_OBJECT


public:
    YouTubeUploader(QObject *parent = 0, const QVariantList &args = QVariantList());
    ~YouTubeUploader();

    void show(const QString &file, QWidget *parent);


private:
    enum WalletWait { None=0, Read, Write };

    KWallet::Wallet *m_wallet;
    WalletWait m_walletWait;
    QHash<QString, QString> m_category;
    QPointer<QWidget> m_dialog;
    UploadThread *m_thread;
    
    void getWallet();
    bool enterWalletFolder(const QString &folder);


private slots:
    void upload();
    void cancelUpload();
    void uploadFinished();
    void quitDialog();
    void threadError(const QString &error);
    void readWallet(bool success);
    void writeWallet(bool success);


};


#endif // YOUTUBEUPLOADER_H
