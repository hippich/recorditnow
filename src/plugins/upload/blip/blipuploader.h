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

#ifndef BLIPUPLOADER_H
#define BLIPUPLOADER_H

// own
#include "abstractuploader.h"
#include "ui_blip.h"
#include "lib/blipvideo.h"

// Qt
#include <QtCore/QVariantList>
#include <QtCore/QPointer>


class BlipService;
class BlipUploader : public AbstractUploader, public Ui::Blip
{
    Q_OBJECT


public:
    BlipUploader(QObject *parent = 0, const QVariantList &args = QVariantList());
    ~BlipUploader();

    void show(const QString &file, QWidget *parent);
    void cancel();


private:
    QHash<QString, QString> m_category;
    QHash<BlipVideo::License, QString> m_license;
    QPointer<QWidget> m_dialog;
    BlipService *m_service;


private slots:
    void upload();
    void cancelUpload();
    void uploadFinished(const KUrl &link, const QString &id);
    void quitDialog();
    void descriptionChanged();
    void addAccount();
    void removeAccount();
    void editAccount();
    void accountsChanged(const QStringList &accounts);
    void currentAccountChanged(const QString &newAccount);
    void gotPasswordForAccount(const QString &account, const QString &password);
    void error(const QString &errorString, const QString &id);


protected:
    bool eventFilter(QObject *watched, QEvent *event);


};


#endif // BLIPUPLOADER_H
