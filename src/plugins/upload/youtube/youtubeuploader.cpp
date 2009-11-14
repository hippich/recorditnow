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
#include <recorditnow_youtube.h>
#include "addaccountdialog.h"

// KDE
#include <klocalizedstring.h>
#include <kplugininfo.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kuiserverjobtracker.h>

// Qt
#include <QtCore/QFile>
#include <QtCore/QDir>



#define GOOGLE QString("By clicking '%1,' you certify that you own all rights to the content or that "\
                "you are authorized by the owner to make the content publicly available on YouTube, "\
                "and that it otherwise complies with the YouTube Terms of Service located at "\
                "http://www.youtube.com/t/terms.")



K_PLUGIN_FACTORY(myFactory, registerPlugin<YouTubeUploader>();)
K_EXPORT_PLUGIN(myFactory("youtube_uploader"))
YouTubeUploader::YouTubeUploader(QObject *parent, const QVariantList &args)
    : AbstractUploader(parent, args)
{

    m_thread = 0;
    m_jobTracker = new KUiServerJobTracker(this);

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

    connect(this, SIGNAL(gotPassword(QString,QString)), this,
            SLOT(gotPasswordForAccount(QString,QString)));

    m_state = Idle;

}


YouTubeUploader::~YouTubeUploader()
{

    if (m_thread) {
        m_jobTracker->unregisterJob(m_thread->getJob());
        m_thread->deleteLater();
    }
    if (m_dialog) {
        delete m_dialog;
    }
    delete m_jobTracker;

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


    accountsChanged(AddAccountDialog::getAccounts());
    connect(accountsCombo, SIGNAL(currentIndexChanged(QString)), this,
            SLOT(currentAccountChanged(QString)));
    currentAccountChanged(accountsCombo->currentText());

    QHashIterator<QString, QString> it(m_category);
    while (it.hasNext()) {
        it.next();
        categoryCombo->addItem(it.value());
    }

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


    QHash<QString, QString> data;
    data["Title"] = titleEdit->text();
    data["Description"] = descriptionEdit->toPlainText();
    data["Tags"] = tagsEdit->text();
    data["Category"] = m_category.key(categoryCombo->currentText());
    data["Login"] = accountsCombo->currentText();
    data["Password"] = passwordEdit->text();
    data["File"] = fileRequester->text();

    QString tags = data["Tags"];
    foreach (const QString &tag, tags.split(',')) {
        if (tag.trimmed().length() > 25) {
            KMessageBox::sorry(m_dialog, i18nc("%1 = youtube tag",
                                               "The Tag %1 is too long.",
                                               tag.trimmed()));
            cancelUpload();
            return;
        }
    }

    QHashIterator<QString, QString> it(data);
    while (it.hasNext()) {
        it.next();
        if (it.value().isEmpty()) {
            KMessageBox::sorry(m_dialog, i18n("Please fill out all the fields."));
            return;
        }
    }

    if (!QFile::exists(fileRequester->text())) {
        KMessageBox::sorry(m_dialog, i18n("No such file: %1", fileRequester->text()));
        return;
    }

    // http://code.google.com/intl/de-DE/apis/youtube/terms.html
    if (KMessageBox::warningContinueCancel(m_dialog, GOOGLE.arg(i18n("Continue"))) != KMessageBox::Continue) {
        cancelUpload();
        return;
    }

    m_thread = new UploadThread(this, data);
    connect(m_thread, SIGNAL(finished()), this, SLOT(uploadFinished()));
    connect(m_thread, SIGNAL(ytError(QString)), this, SLOT(threadError(QString)));

    setState(Upload);
    m_jobTracker->registerJob(m_thread->getJob());
    m_thread->start();

}


void YouTubeUploader::cancelUpload()
{

    if (m_thread) {
        kDebug() << "cancel";
        m_jobTracker->unregisterJob(m_thread->getJob());
        m_thread->cancelUpload();
        m_thread = 0;
    }
    setState(Idle);

}


void YouTubeUploader::uploadFinished()
{

    qDebug() << "upload finished";
    m_jobTracker->unregisterJob(m_thread->getJob());
    m_thread->deleteLater();
    m_thread = 0;
    setState(Idle);

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
    kDebug() << text.length();
    if (text.length() > 5000) {
        text.resize(5000);
        descriptionEdit->setText(text);
    }

}


void YouTubeUploader::addAccount()
{

    AddAccountDialog *dialog = new AddAccountDialog(m_dialog);
    connect(dialog, SIGNAL(accountsChanged(QStringList)), this, SLOT(accountsChanged(QStringList)));
    connect(dialog, SIGNAL(newPassword(QString,QString)), this, SLOT(newPassword(QString,QString)));
    dialog->show();

}


void YouTubeUploader::removeAccount()
{

    const QString account = accountsCombo->currentText();
    if (!account.isEmpty()) {
        AddAccountDialog::removeAccount(account);
        accountsCombo->removeItem(accountsCombo->currentIndex());
    }

}


void YouTubeUploader::editAccount()
{

    if (accountsCombo->currentText().isEmpty()) {
        return;
    }

    AddAccountDialog *dialog = new AddAccountDialog(m_dialog, this, accountsCombo->currentText());
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


void YouTubeUploader::newPassword(const QString &account, const QString &password)
{

    setPassword(account, password);

}


void YouTubeUploader::currentAccountChanged(const QString &newAccount)
{

    passwordEdit->clear();
    if (!newAccount.isEmpty() && AddAccountDialog::hasPassword(newAccount)) {
        getPassword(newAccount);
    }

}


void YouTubeUploader::gotPasswordForAccount(const QString &account, const QString &password)
{

    if (m_dialog && accountsCombo->currentText() == account) {
        passwordEdit->setText(password);
    }

}
