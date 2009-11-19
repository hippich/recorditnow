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

    m_license[BlipVideo::NoLicense] = i18n("No License");
    m_license[BlipVideo::CreativeCommonsAttribution_2_0] = "Creative Commons Attribution 2.0";
    m_license[BlipVideo::CreativeCommonsAttribution_NoDerivs_2_0] =
            "Creative Commons Attribution-NoDerivs 2.0";
    m_license[BlipVideo::CreativeCommonsAttribution_NonCommercial_NoDerivs_2_0] =
            "Creative Commons Attribution-NonCommercial-NoDerivs 2.0";
    m_license[BlipVideo::CreativeCommonsAttribution_NonCommercial_2_0] =
            "Creative Commons Attribution-NonCommercial 2.0";
    m_license[BlipVideo::CreativeCommonsAttribution_NonCommercial_ShareAlike_2_0] =
            "Creative Commons Attribution-NonCommercial-ShareAlike 2.0";
    m_license[BlipVideo::CreativeCommonsAttribution_ShareAlike_2_0] =
            "Creative Commons Attribution-ShareAlike 2.0";
    m_license[BlipVideo::PublicDomain] = "Public Domain";
    m_license[BlipVideo::CreativeCommonsAttribution_3_0] = "Creative Commons Attribution 3.0";
    m_license[BlipVideo::CreativeCommonsAttribution_NoDerivs_3_0] =
            "Creative Commons Attribution-NoDerivs 3.0";
    m_license[BlipVideo::CreativeCommonsAttribution_NonCommercial_NoDerivs_3_0] =
            "Creative Commons Attribution-NonCommercial-NoDerivs 3.0";
    m_license[BlipVideo::CreativeCommonsAttribution_NonCommercial_3_0] =
            "Creative Commons Attribution-NonCommercial 3.0";
    m_license[BlipVideo::CreativeCommonsAttribution_NonCommercial_ShareAlike_3_0] =
            "Creative Commons Attribution-NonCommercial-ShareAlike 3.0";
    m_license[BlipVideo::CreativeCommonsAttribution_ShareAlike_3_0] =
            "Creative Commons Attribution-ShareAlike 3.0";


    m_category[BlipVideo::Art] = i18n("Art");
    m_category[BlipVideo::AutosAndVehicles] = i18n("Autos & Vehicles");
    m_category[BlipVideo::Business] = i18n("Business");
    m_category[BlipVideo::CitizenJournalism] = i18n("Citizen Journalism");
    m_category[BlipVideo::Comedy] = i18n("Comedy");
    m_category[BlipVideo::ConferencesAndOtherEvents] = i18n("Conferences and other Events");
    m_category[BlipVideo::DefaultCategory] = i18n("Default Category");
    m_category[BlipVideo::Documentary] = i18n("Documentary");
    m_category[BlipVideo::Educational] = i18n("Educational");
    m_category[BlipVideo::FoodAndDrink] = i18n("Food & Drink");
    m_category[BlipVideo::Friends] = i18n("Friends");
    m_category[BlipVideo::Gaming] = i18n("Gaming");
    m_category[BlipVideo::Health] = i18n("Health");
    m_category[BlipVideo::Literature] = i18n("Literature");
    m_category[BlipVideo::MoviesAndTelevision] = i18n("Movies and Television");
    m_category[BlipVideo::MusicAndEntertainment] = i18n("Music and Entertainment");
    m_category[BlipVideo::PersonalOrAutoBiographical] = i18n("Personal or Auto-biographical");
    m_category[BlipVideo::Politics] = i18n("Politics");
    m_category[BlipVideo::Religion] = i18n("Religion");
    m_category[BlipVideo::SchoolAndEducation] = i18n("School and Education");
    m_category[BlipVideo::Science] = i18n("Science");
    m_category[BlipVideo::Sports] = i18n("Sports");
    m_category[BlipVideo::Technology] = i18n("Technology");
    m_category[BlipVideo::TheEnvironment] = i18n("The Environment");
    m_category[BlipVideo::TheMainstreamMedia] = i18n("The Mainstream Media");
    m_category[BlipVideo::Travel] = i18n("Travel");
    m_category[BlipVideo::Videoblogging] = i18n("Videoblogging");
    m_category[BlipVideo::WebDevelopmentAndSites] = i18n("Web Development and Sites");


    connect(this, SIGNAL(gotPassword(QString,QString)), this,
            SLOT(gotPasswordForAccount(QString,QString)));

    m_state = Idle;

}


BlipUploader::~BlipUploader()
{

    if (m_dialog) {
        delete m_dialog;
    }
    if (m_service) {
        delete m_service;
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


    accountsChanged(AddAccountDialog::getAccounts(Settings::self(), "blip_accounts"));
    connect(accountsCombo, SIGNAL(currentIndexChanged(QString)), this,
            SLOT(currentAccountChanged(QString)));
    currentAccountChanged(accountsCombo->currentText());


    QHashIterator<BlipVideo::License, QString> lIt(m_license);
    while (lIt.hasNext()) {
        lIt.next();
        licenseCombo->addItem(lIt.value());
    }

    QHashIterator<BlipVideo::Category, QString> cIt(m_category);
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

    BlipVideo video;
    video.setTitle(titleEdit->text());
    video.setDescription(descriptionEdit->toPlainText());
    video.setCategory(m_category.key(categoryCombo->currentText()));
    video.setLicense(m_license.key(licenseCombo->currentText()));
    video.setFile(fileRequester->text());
    video.setKeywords(tagsEdit->text());

    m_service = new BlipService(this);

    connect(m_service, SIGNAL(uploadFinished(QString)), this, SLOT(uploadFinished(QString)));
    connect(m_service, SIGNAL(error(QString, QString)), this, SLOT(error(QString, QString)));
    connect(m_service, SIGNAL(canceled(QString)), this, SLOT(uploadFinished(QString)));

    setState(Upload);
    QString id = m_service->upload(&video, accountsCombo->currentText(), passwordEdit->text());

    if (id.isEmpty()) {
        cancelUpload();
    }

}


void BlipUploader::cancelUpload()
{

    if (m_service) {
        delete m_service;
        m_service = 0;
    }
    setState(Idle);

}


void BlipUploader::uploadFinished(const QString &id)
{

    qDebug() << "upload finished" << id;
    if (m_service) {
        delete m_service;
        m_service = 0;
    }
    setState(Idle);

}


void BlipUploader::error(const QString &errorString, const QString &id)
{

    Q_UNUSED(id);

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
