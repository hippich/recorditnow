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
#include <recorditnow_youtube.h>
#include "addaccountdialog.h"
#include "lib/youtubeservice.h"
#include "lib/youtubevideo.h"

// KDE
#include <klocalizedstring.h>
#include <kplugininfo.h>
#include <kdebug.h>
#include <kmessagebox.h>

// Qt
#include <QtCore/QFile>
#include <QtCore/QDir>



#define GOOGLE                 "By clicking &apos;Continue&apos;, you certify that you own all rights to "\
                               "the content or that you are authorized by the owner to make the "\
                               "content publicly available on YouTube, and that it otherwise "\
                               "complies with the YouTube Terms of Service located at "\
                               "<a href='http://www.youtube.com/t/terms'>"\
                               "http://www.youtube.com/t/terms</a>."

static QString GOOGLETR = i18n("By clicking 'Continue', you certify that you own all rights to "
                               "the content or that you are authorized by the owner to make the "
                               "content publicly available on YouTube, and that it otherwise "
                               "complies with the YouTube Terms of Service located at "
                               "<a href='http://www.youtube.com/t/terms'>"
                               "http://www.youtube.com/t/terms</a>.");


K_PLUGIN_FACTORY(myFactory, registerPlugin<YouTubeUploader>();)
K_EXPORT_PLUGIN(myFactory("youtube_uploader"))
YouTubeUploader::YouTubeUploader(QObject *parent, const QVariantList &args)
    : AbstractUploader(parent, args)
{

    m_service = 0;

    connect(this, SIGNAL(gotPassword(QString,QString)), this,
            SLOT(gotPasswordForAccount(QString,QString)));

    m_state = Idle;

}


YouTubeUploader::~YouTubeUploader()
{

    if (m_service) {
        delete m_service;
    }
    if (m_dialog) {
        delete m_dialog;
    }

}


void YouTubeUploader::show(const QString &file, QWidget *parent)
{

    if (m_dialog || m_state != Idle) {
        return;
    }

    setId(parent->winId(), "Youtube");

    m_dialog = new QWidget(parent, Qt::Dialog);
    m_dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    m_dialog->setWindowIcon(KIcon("recorditnow_youtube"));
    setupUi(m_dialog);

    const QPixmap pixmap = KIcon("recorditnow_youtube").pixmap(KIconLoader::SizeLarge,
                                                               KIconLoader::SizeLarge);
    titleWidget->setPixmap(pixmap, KTitleWidget::ImageLeft);
    
    uploadButton->setIcon(KIcon("recorditnow-upload-media"));
    cancelButton->setIcon(KIcon("dialog-cancel"));
    quitButton->setIcon(KIcon("dialog-close"));

    fileRequester->setText(file);

    connect(uploadButton, SIGNAL(clicked()), this, SLOT(upload()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(quitDialog()));
    connect(descriptionEdit, SIGNAL(textChanged()), this, SLOT(descriptionChanged()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelUpload()));

    addAccountButton->setIcon(KIcon("list-add"));
    editAccountButton->setIcon(KIcon("document-edit"));
    removeAccountButton->setIcon(KIcon("list-remove"));

    connect(addAccountButton, SIGNAL(clicked()), this, SLOT(addAccount()));
    connect(editAccountButton, SIGNAL(clicked()), this, SLOT(editAccount()));
    connect(removeAccountButton, SIGNAL(clicked()), this, SLOT(removeAccount()));


    accountsChanged(AddAccountDialog::getAccounts(Settings::self(), "youtube_accounts"));
    connect(accountsCombo, SIGNAL(currentIndexChanged(QString)), this,
            SLOT(currentAccountChanged(QString)));
    currentAccountChanged(accountsCombo->currentText());

    YouTubeVideo video;
    categoryCombo->addItems(video.categorys());

    Settings::self()->readConfig();
    accountsCombo->setCurrentItem(Settings::currentAccount(), false);

    setState(Idle);
    m_dialog->show();

}


void YouTubeUploader::upload()
{

    kDebug() << "upload";

    Settings::self()->setCurrentAccount(accountsCombo->currentText());
    Settings::self()->writeConfig();


    // http://code.google.com/intl/de-DE/apis/youtube/terms.html
    QString message(GOOGLE);
    if (QString(GOOGLE) != GOOGLETR) {
        message.append("<br>");
        message.append("<br>");
        message.append("<br>");
        message.append(i18n("Translation:"));
        message.append("<br>");
        message.append("<br>");
        message.append(GOOGLETR);
    }

    if (KMessageBox::warningContinueCancel(m_dialog, message) != KMessageBox::Continue) {
        cancelUpload();
        return;
    }

    m_service = new YouTubeService(this);
    connect(m_service, SIGNAL(authenticated(QString)), this, SLOT(authenticated(QString)));
    connect(m_service, SIGNAL(uploadFinished(QString)), this, SLOT(uploadFinished(QString)));
    connect(m_service, SIGNAL(error(QString, QString)), this, SLOT(serviceError(QString, QString)));
    connect(m_service, SIGNAL(canceled(QString)), this, SLOT(uploadFinished(QString)));

    setState(Upload);
    QString id = m_service->authenticate(accountsCombo->currentText(), passwordEdit->text());

    if (id.startsWith("Error: ")) {
        serviceError(id.remove(0, 7), QString());
    }

}


void YouTubeUploader::cancelUpload()
{

    if (m_service) {
        kDebug() << "cancel";
        delete m_service;
        m_service = 0;
    }
    setState(Idle);

}


void YouTubeUploader::uploadFinished(const QString &account)
{

    qDebug() << "upload finished" << account;
    delete m_service;
    m_service = 0;
    setState(Idle);

}


void YouTubeUploader::quitDialog()
{

    cancelUpload();
    m_dialog->close();
    emit finished();

}


void YouTubeUploader::authenticated(const QString &account)
{

    kDebug() << "auth finished for:" << account;

    YouTubeVideo video;
    video.setTitle(titleEdit->text());
    video.setDescription(descriptionEdit->toPlainText());
    video.setKeywords(tagsEdit->text());
    video.setCategory(categoryCombo->currentText());
    video.setFile(fileRequester->text());

    QString id = m_service->upload(&video, accountsCombo->currentText());

    if (id.startsWith("Error:")) {
        serviceError(id.remove(0, 7), QString());
    }

}


void YouTubeUploader::serviceError(const QString &error, const QString &id)
{

    kDebug() << "error:" << id;
    KMessageBox::error(m_dialog, error);
    cancelUpload();

}


void YouTubeUploader::setState(const State &state)
{

    if (!m_dialog) {
        return;
    }

    switch (state) {
    case Idle: {
            uploadButton->setEnabled(true);
            cancelButton->setEnabled(false);
            quitButton->setEnabled(true);
            break;
        }
    case Upload: {
            uploadButton->setEnabled(false);
            cancelButton->setEnabled(true);
            quitButton->setEnabled(true);
            break;
        }
    }

}


void YouTubeUploader::descriptionChanged()
{

    QString text = descriptionEdit->toPlainText();
    if (text.length() > 5000) {
        text.resize(5000);
        descriptionEdit->setText(text);
    }

}


void YouTubeUploader::addAccount()
{

    AddAccountDialog *dialog = new AddAccountDialog(m_dialog, Settings::self(), "recorditnow_youtube",
                                                    "youtube_accounts");
    connect(dialog, SIGNAL(accountsChanged(QStringList)), this, SLOT(accountsChanged(QStringList)));
    connect(dialog, SIGNAL(newPassword(QString,QString)), this, SLOT(newPassword(QString,QString)));
    dialog->show();

}


void YouTubeUploader::removeAccount()
{

    const QString account = accountsCombo->currentText();
    if (!account.isEmpty()) {
        AddAccountDialog::removeAccount(account, Settings::self(), "youtube_accounts");
        accountsCombo->removeItem(accountsCombo->currentIndex());
    }

}


void YouTubeUploader::editAccount()
{

    if (accountsCombo->currentText().isEmpty()) {
        return;
    }

    AddAccountDialog *dialog = new AddAccountDialog(m_dialog, Settings::self(), "recorditnow_youtube",
                                                    "youtube_accounts",
                                                    this,
                                                    accountsCombo->currentText());
    connect(dialog, SIGNAL(accountsChanged(QStringList)), this, SLOT(accountsChanged(QStringList)));
    connect(dialog, SIGNAL(newPassword(QString,QString)), this, SLOT(newPassword(QString,QString)));
    dialog->show();

}


void YouTubeUploader::accountsChanged(const QStringList &accounts)
{

    const QString current = accountsCombo->currentText();
    accountsCombo->clear();
    accountsCombo->addItems(accounts);
    accountsCombo->setCurrentItem(current, false);

}


void YouTubeUploader::currentAccountChanged(const QString &newAccount)
{

    passwordEdit->clear();
    if (!newAccount.isEmpty() && AddAccountDialog::hasPassword(newAccount, Settings::self(),
                                                               "youtube_accounts")) {
        getPassword(newAccount);
    }

}


void YouTubeUploader::gotPasswordForAccount(const QString &account, const QString &password)
{

    if (m_dialog && accountsCombo->currentText() == account) {
        passwordEdit->setText(password);
    }

}
