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
#include "blipuploader.h"
#include <recorditnow_blip.h>
#include "addaccountdialog.h"
#include "lib/blipvideo.h"
#include "lib/blipservice.h"

// KDE
#include <klocalizedstring.h>
#include <kplugininfo.h>
#include <kdebug.h>
#include <kmessagebox.h>

// Qt
#include <QtCore/QFile>
#include <QtCore/QDir>



K_PLUGIN_FACTORY(myFactory, registerPlugin<BlipUploader>();)
K_EXPORT_PLUGIN(myFactory("recorditnow_blip"))
BlipUploader::BlipUploader(QObject *parent, const QVariantList &args)
    : AbstractUploader(parent, args)
{

    m_service = 0;

    connect(this, SIGNAL(gotPassword(QString,QString)), this,
            SLOT(gotPasswordForAccount(QString,QString)));

}


BlipUploader::~BlipUploader()
{

    if (m_dialog) {
        delete m_dialog;
    }
    if (m_service) {
        m_service->cancelUpload();
        m_service->deleteLater();
    }

}


void BlipUploader::show(const QString &file, QWidget *parent)
{

    if (m_dialog) {
        return;
    }

    setId(parent->winId(), "blip");

    m_dialog = new QWidget(parent, Qt::Dialog);
    m_dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    m_dialog->setWindowIcon(KIcon("recorditnow_blip"));
    setupUi(m_dialog);

    const QPixmap pixmap = KIcon("recorditnow_blip").pixmap(KIconLoader::SizeLarge,
                                                               KIconLoader::SizeLarge);
    titleWidget->setPixmap(pixmap, KTitleWidget::ImageLeft);

    uploadButton->setIcon(KIcon("recorditnow-upload-media"));
    quitButton->setIcon(KIcon("dialog-close"));

    fileRequester->setText(file);

    connect(uploadButton, SIGNAL(clicked()), this, SLOT(upload()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(quitDialog()));
    connect(descriptionEdit, SIGNAL(textChanged()), this, SLOT(descriptionChanged()));

    addAccountButton->setIcon(KIcon("list-add"));
    editAccountButton->setIcon(KIcon("document-edit"));
    removeAccountButton->setIcon(KIcon("list-remove"));

    connect(addAccountButton, SIGNAL(clicked()), this, SLOT(addAccount()));
    connect(editAccountButton, SIGNAL(clicked()), this, SLOT(editAccount()));
    connect(removeAccountButton, SIGNAL(clicked()), this, SLOT(removeAccount()));


    accountsChanged(AddAccountDialog::getAccounts(Settings::self(), "blip_accounts"));
    connect(accountsCombo, SIGNAL(currentIndexChanged(QString)), this,
            SLOT(currentAccountChanged(QString)));
    currentAccountChanged(accountsCombo->currentText());


    BlipVideo video;
    licenseCombo->addItems(video.licenses());
    categoryCombo->addItems(video.categorys());

    Settings::self()->readConfig();
    accountsCombo->setCurrentItem(Settings::currentAccount(), false);

    m_dialog->show();

}


void BlipUploader::cancel()
{

    cancelUpload();
    emit status(i18n("Upload canceled."));
    emit finished(QString());

}


void BlipUploader::upload()
{

    kDebug() << "upload";

    Settings::self()->setCurrentAccount(accountsCombo->currentText());
    Settings::self()->writeConfig();

    BlipVideo video;
    video.setTitle(titleEdit->text());
    video.setDescription(descriptionEdit->toPlainText());
    video.setCategory(categoryCombo->currentText());
    video.setLicense(licenseCombo->currentText());
    video.setFile(fileRequester->text());
    video.setKeywords(tagsEdit->text());

    m_dialog->close();

    m_service = new BlipService(this);

    connect(m_service, SIGNAL(uploadFinished(KUrl, QString)), this, SLOT(uploadFinished(KUrl, QString)));
    connect(m_service, SIGNAL(error(QString, QString)), this, SLOT(error(QString, QString)));
    connect(m_service, SIGNAL(canceled(QString)), this, SLOT(uploadFinished(QString)));

    emit status(i18n("Uploading..."));

    QString id = m_service->upload(&video, accountsCombo->currentText(), passwordEdit->text());

    if (id.startsWith("Error:")) {
        error(id.remove(0, 7), QString());
    }

}


void BlipUploader::cancelUpload()
{

    if (m_service) {
        m_service->cancelUpload();
        m_service->deleteLater();;
        m_service = 0;
    }

}


void BlipUploader::uploadFinished(const KUrl &link, const QString &id)
{

    qDebug() << "upload finished" << id;
    if (m_service) {
        m_service->deleteLater();
        m_service = 0;
    }

    QString html = "<a href='%1'>%1</a>";
    emit status(i18n("Finished: %1", html.arg(link.pathOrUrl())));
    emit finished(QString());

}


void BlipUploader::error(const QString &errorString, const QString &id)
{

    Q_UNUSED(id);
    cancelUpload();
    emit finished(errorString);

}


void BlipUploader::quitDialog()
{

    cancelUpload();
    m_dialog->close();
    emit finished(QString());

}


void BlipUploader::descriptionChanged()
{

    QString text = descriptionEdit->toPlainText();
    if (text.length() > 5000) {
        text.resize(5000);
        descriptionEdit->setText(text);
    }

}


void BlipUploader::addAccount()
{

    AddAccountDialog *dialog = new AddAccountDialog(m_dialog, Settings::self(), "recorditnow_blip",
                                                    "blip_accounts");
    connect(dialog, SIGNAL(accountsChanged(QStringList)), this, SLOT(accountsChanged(QStringList)));
    connect(dialog, SIGNAL(newPassword(QString,QString)), this, SLOT(newPassword(QString,QString)));
    dialog->show();

}


void BlipUploader::removeAccount()
{

    const QString account = accountsCombo->currentText();
    if (!account.isEmpty()) {
        AddAccountDialog::removeAccount(account, Settings::self(), "blip_accounts");
        accountsCombo->removeItem(accountsCombo->currentIndex());
    }

}


void BlipUploader::editAccount()
{

    if (accountsCombo->currentText().isEmpty()) {
        return;
    }

    AddAccountDialog *dialog = new AddAccountDialog(m_dialog, Settings::self(), "recorditnow_blip",
                                                    "blip_accounts",
                                                    this,
                                                    accountsCombo->currentText());
    connect(dialog, SIGNAL(accountsChanged(QStringList)), this, SLOT(accountsChanged(QStringList)));
    connect(dialog, SIGNAL(newPassword(QString,QString)), this, SLOT(newPassword(QString,QString)));
    dialog->show();

}


void BlipUploader::accountsChanged(const QStringList &accounts)
{

    const QString current = accountsCombo->currentText();
    accountsCombo->clear();
    accountsCombo->addItems(accounts);
    accountsCombo->setCurrentItem(current, false);

}


void BlipUploader::currentAccountChanged(const QString &newAccount)
{

    passwordEdit->clear();
    if (!newAccount.isEmpty() && AddAccountDialog::hasPassword(newAccount, Settings::self(),
                                                               "blip_accounts")) {
        getPassword(newAccount);
    }

}


void BlipUploader::gotPasswordForAccount(const QString &account, const QString &password)
{

    if (m_dialog && accountsCombo->currentText() == account) {
        passwordEdit->setText(password);
    }

}
