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
#include "youtubeuploader.h"
#include "uploadthread.h"

// KDE
#include <klocalizedstring.h>
#include <kplugininfo.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kmessagebox.h>

// Qt
#include <QtCore/QFile>
#include <QtCore/QDir>



#define GOOGLE QString("By clicking '%1,' you certify that you own all rights to the content or that " \
                "you are authorized by the owner to make the content publicly available on YouTube, " \
                "and that it otherwise complies with the YouTube Terms of Service located at " \
                "http://www.youtube.com/t/terms.")



K_PLUGIN_FACTORY(myFactory, registerPlugin<YouTubeUploader>();)
K_EXPORT_PLUGIN(myFactory("youtube_uploader"))
YouTubeUploader::YouTubeUploader(QObject *parent, const QVariantList &args)
    : AbstractUploader(parent, args)
{

    m_thread = 0;

    m_category["Autos"] = i18n("Autos & Vehicles");
    m_category["Comedy"] = i18n("Comedy");
    m_category["Education"] = i18n("Education");
    m_category["Entertainment"] = i18n("Entertainment");
    m_category["Film"] = i18n("Film & Animation");
    m_category["Games"] = i18n("Gaming");
    m_category["Howto"] = i18n("Howto & Style");
    m_category["Music"] = i18n("Music");
    m_category["News"] = i18n("News & Politics");
    m_category["Nonprofit"] = i18n("Nonprofit & Activism");
    m_category["People"] = i18n("People & Blogs");
    m_category["Animals"] = i18n("Pets & Animals");
    m_category["Tech"] = i18n("Sience & Technology");
    m_category["Sports"] = i18n("Sports");
    m_category["Travel"] = i18n("Travel & Evends");

}


YouTubeUploader::~YouTubeUploader()
{

    if (m_thread) {
        m_thread->deleteLater();
    }
    if (m_dialog) {
        delete m_dialog;
    }

}


void YouTubeUploader::show(const QString &file, QWidget *parent)
{

    if (m_dialog) {
        return;
    }

    m_dialog = new QWidget(parent, Qt::Dialog);
    m_dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    m_dialog->setWindowIcon(KIcon("recorditnow_youtube"));
    setupUi(m_dialog);

    fileRequester->setText(file);

    connect(uploadButton, SIGNAL(clicked()), this, SLOT(upload()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(quitDialog()));

    QHashIterator<QString, QString> it(m_category);
    while (it.hasNext()) {
        it.next();
        categoryCombo->addItem(it.value());
    }
    m_dialog->show();

}


void YouTubeUploader::upload()
{

    kDebug() << "upload";
    uploadButton->setDisabled(true);

    // http://code.google.com/intl/de-DE/apis/youtube/terms.html
    if (KMessageBox::warningContinueCancel(m_dialog, GOOGLE.arg(i18n("Continue"))) != KMessageBox::Continue) {
        cancelUpload();
        return;
    }

    QHash<QString, QString> data;
    data["Title"] = titleEdit->text();
    data["Description"] = descriptionEdit->text();
    data["Tags"] = tagsEdit->text();
    data["Category"] = m_category.key(categoryCombo->currentText());
    data["Login"] = loginEdit->text();
    data["Password"] = passwordEdit->text();
    data["File"] = fileRequester->text();

    QString tags = data["Tags"];
    foreach (const QString &tag, tags.split(',')) {
        if (tag.trimmed().length() > 25) {
            KMessageBox::sorry(m_dialog, i18n("The Tag %1 is too long.", tag.trimmed()));
            cancelUpload();
            return;
        }
    }

    QHashIterator<QString, QString> it(data);
    while (it.hasNext()) {
        it.next();
        if (it.value().isEmpty()) {
            m_dialog->setDisabled(false);
            KMessageBox::sorry(m_dialog, i18n(""));
            return;
        }
    }

    if (!QFile::exists(fileRequester->text())) {
        m_dialog->setDisabled(false);
        KMessageBox::sorry(m_dialog, i18n(""));
        return;
    }

    m_thread = new UploadThread(this, data);
    connect(m_thread, SIGNAL(finished()), this, SLOT(uploadFinished()));
    connect(m_thread, SIGNAL(ytError(QString)), this, SLOT(threadError(QString)));

    progressBar->setMaximum(0);
    m_thread->start();

}

// TODO
void YouTubeUploader::cancelUpload()
{

    kDebug() << "cancel";
    progressBar->setMaximum(1);
    uploadButton->setDisabled(false);
    if (m_thread) {
        m_thread->deleteLater();
        m_thread = 0;
    }

}


void YouTubeUploader::uploadFinished()
{

    qDebug() << "upload finished";
    progressBar->setMaximum(1);
    m_thread->deleteLater();
    m_thread = 0;
    if (m_dialog) {
        uploadButton->setDisabled(false);
    }

}


void YouTubeUploader::quitDialog()
{

    cancelUpload();
    m_dialog->close();

}


void YouTubeUploader::threadError(const QString &error)
{

    KMessageBox::error(m_dialog, error);
    cancelUpload();

}
