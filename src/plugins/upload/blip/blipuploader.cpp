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
#include "lib/kblipvideo.h"
#include "lib/kblipaccount.h"

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

    m_video = 0;

    m_license[KBlipVideo::NoLicense] = i18n("No License");
    m_license[KBlipVideo::CreativeCommonsAttribution_2_0] = "Creative Commons Attribution 2.0";
    m_license[KBlipVideo::CreativeCommonsAttribution_NoDerivs_2_0] =
            "Creative Commons Attribution-NoDerivs 2.0";
    m_license[KBlipVideo::CreativeCommonsAttribution_NonCommercial_NoDerivs_2_0] =
            "Creative Commons Attribution-NonCommercial-NoDerivs 2.0";
    m_license[KBlipVideo::CreativeCommonsAttribution_NonCommercial_2_0] =
            "Creative Commons Attribution-NonCommercial 2.0";
    m_license[KBlipVideo::CreativeCommonsAttribution_NonCommercial_ShareAlike_2_0] =
            "Creative Commons Attribution-NonCommercial-ShareAlike 2.0";
    m_license[KBlipVideo::CreativeCommonsAttribution_ShareAlike_2_0] =
            "Creative Commons Attribution-ShareAlike 2.0";
    m_license[KBlipVideo::PublicDomain] = "Public Domain";
    m_license[KBlipVideo::CreativeCommonsAttribution_3_0] = "Creative Commons Attribution 3.0";
    m_license[KBlipVideo::CreativeCommonsAttribution_NoDerivs_3_0] =
            "Creative Commons Attribution-NoDerivs 3.0";
    m_license[KBlipVideo::CreativeCommonsAttribution_NonCommercial_NoDerivs_3_0] =
            "Creative Commons Attribution-NonCommercial-NoDerivs 3.0";
    m_license[KBlipVideo::CreativeCommonsAttribution_NonCommercial_3_0] =
            "Creative Commons Attribution-NonCommercial 3.0";
    m_license[KBlipVideo::CreativeCommonsAttribution_NonCommercial_ShareAlike_3_0] =
            "Creative Commons Attribution-NonCommercial-ShareAlike 3.0";
    m_license[KBlipVideo::CreativeCommonsAttribution_ShareAlike_3_0] =
            "Creative Commons Attribution-ShareAlike 3.0";


    m_category[KBlipVideo::Art] = i18n("Art");
    m_category[KBlipVideo::AutosAndVehicles] = i18n("Autos & Vehicles");
    m_category[KBlipVideo::Business] = i18n("Business");
    m_category[KBlipVideo::CitizenJournalism] = i18n("Citizen Journalism");
    m_category[KBlipVideo::Comedy] = i18n("Comedy");
    m_category[KBlipVideo::ConferencesAndOtherEvents] = i18n("Conferences and other Events");
    m_category[KBlipVideo::DefaultCategory] = i18n("Default Category");
    m_category[KBlipVideo::Documentary] = i18n("Documentary");
    m_category[KBlipVideo::Educational] = i18n("Educational");
    m_category[KBlipVideo::FoodAndDrink] = i18n("Food & Drink");
    m_category[KBlipVideo::Friends] = i18n("Friends");
    m_category[KBlipVideo::Gaming] = i18n("Gaming");
    m_category[KBlipVideo::Health] = i18n("Health");
    m_category[KBlipVideo::Literature] = i18n("Literature");
    m_category[KBlipVideo::MoviesAndTelevision] = i18n("Movies and Television");
    m_category[KBlipVideo::MusicAndEntertainment] = i18n("Music and Entertainment");
    m_category[KBlipVideo::PersonalOrAutoBiographical] = i18n("Personal or Auto-biographical");
    m_category[KBlipVideo::Politics] = i18n("Politics");
    m_category[KBlipVideo::Religion] = i18n("Religion");
    m_category[KBlipVideo::SchoolAndEducation] = i18n("School and Education");
    m_category[KBlipVideo::Science] = i18n("Science");
    m_category[KBlipVideo::Sports] = i18n("Sports");
    m_category[KBlipVideo::Technology] = i18n("Technology");
    m_category[KBlipVideo::TheEnvironment] = i18n("The Environment");
    m_category[KBlipVideo::TheMainstreamMedia] = i18n("The Mainstream Media");
    m_category[KBlipVideo::Travel] = i18n("Travel");
    m_category[KBlipVideo::Videoblogging] = i18n("Videoblogging");
    m_category[KBlipVideo::WebDevelopmentAndSites] = i18n("Web Development and Sites");


    connect(this, SIGNAL(gotPassword(QString,QString)), this,
            SLOT(gotPasswordForAccount(QString,QString)));

    m_state = Idle;

}


BlipUploader::~BlipUploader()
{

    if (m_dialog) {
        delete m_dialog;
    }
    if (m_video) {
        delete m_video;
    }

}


void BlipUploader::show(const QString &file, QWidget *parent)
{

    if (m_dialog || m_state != Idle) {
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


    accountsChanged(AddAccountDialog::getAccounts(Settings::self()));
    connect(accountsCombo, SIGNAL(currentIndexChanged(QString)), this,
            SLOT(currentAccountChanged(QString)));
    currentAccountChanged(accountsCombo->currentText());


    QHashIterator<KBlipVideo::License, QString> lIt(m_license);
    while (lIt.hasNext()) {
        lIt.next();
        licenseCombo->addItem(lIt.value());
    }

    QHashIterator<KBlipVideo::Category, QString> cIt(m_category);
    while (cIt.hasNext()) {
        cIt.next();
        categoryCombo->addItem(cIt.value());
    }

    Settings::self()->readConfig();
    accountsCombo->setCurrentItem(Settings::currentAccount(), false);

    setState(Idle);
    m_dialog->show();

}


void BlipUploader::upload()
{

    kDebug() << "upload";

    Settings::self()->setCurrentAccount(accountsCombo->currentText());
    Settings::self()->writeConfig();

    KBlipAccount account;
    account.setUsername(accountsCombo->currentText());
    account.setPassword(passwordEdit->text());

    m_video = new KBlipVideo(this);
    m_video->setTitle(titleEdit->text());
    m_video->setDescription(descriptionEdit->toPlainText());
    m_video->setCategory(m_category.key(categoryCombo->currentText()));
    m_video->setLicense(m_license.key(licenseCombo->currentText()));
    m_video->setFile(fileRequester->text());
    m_video->setTags(tagsEdit->text().split(','));

    connect(m_video, SIGNAL(finished()), this, SLOT(uploadFinished()));
    connect(m_video, SIGNAL(error(QString)), this, SLOT(error(QString)));

    setState(Upload);
    m_video->send(&account);

}


void BlipUploader::cancelUpload()
{

    if (m_video) {
        delete m_video;
        m_video = 0;
    }
    setState(Idle);

}


void BlipUploader::uploadFinished()
{

    qDebug() << "upload finished";
    if (m_video) {
        delete m_video;
        m_video = 0;
    }
    setState(Idle);

}


void BlipUploader::error(const QString &errorString)
{

    KMessageBox::error(m_dialog, errorString);
    cancelUpload();

}


void BlipUploader::quitDialog()
{

    cancelUpload();
    m_dialog->close();
    emit finished();

}


void BlipUploader::setState(const State &state)
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


void BlipUploader::descriptionChanged()
{

    QString text = descriptionEdit->toPlainText();
    kDebug() << text.length();
    if (text.length() > 5000) {
        text.resize(5000);
        descriptionEdit->setText(text);
    }

}


void BlipUploader::addAccount()
{

    AddAccountDialog *dialog = new AddAccountDialog(m_dialog, Settings::self());
    connect(dialog, SIGNAL(accountsChanged(QStringList)), this, SLOT(accountsChanged(QStringList)));
    connect(dialog, SIGNAL(newPassword(QString,QString)), this, SLOT(newPassword(QString,QString)));
    dialog->show();

}


void BlipUploader::removeAccount()
{

    const QString account = accountsCombo->currentText();
    if (!account.isEmpty()) {
        AddAccountDialog::removeAccount(account, Settings::self());
        accountsCombo->removeItem(accountsCombo->currentIndex());
    }

}


void BlipUploader::editAccount()
{

    if (accountsCombo->currentText().isEmpty()) {
        return;
    }

    AddAccountDialog *dialog = new AddAccountDialog(m_dialog, Settings::self(), this,
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


void BlipUploader::newPassword(const QString &account, const QString &password)
{

    setPassword(account, password);

}


void BlipUploader::currentAccountChanged(const QString &newAccount)
{

    passwordEdit->clear();
    if (!newAccount.isEmpty() && AddAccountDialog::hasPassword(newAccount, Settings::self())) {
        getPassword(newAccount);
    }

}


void BlipUploader::gotPasswordForAccount(const QString &account, const QString &password)
{

    if (m_dialog && accountsCombo->currentText() == account) {
        passwordEdit->setText(password);
    }

}
