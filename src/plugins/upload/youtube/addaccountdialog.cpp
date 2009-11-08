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
#include "addaccountdialog.h"
#include <recorditnow_youtube.h>
#include "abstractuploader.h"

// KDE
#include <klocalizedstring.h>
#include <kicon.h>


AddAccountDialog::AddAccountDialog(QWidget *parent, AbstractUploader *uploader,
                                   const QString &account)
    : KDialog(parent)
{

    setWindowIcon(KIcon("recorditnow_youtube"));
    setWindowTitle(i18n("Add a new Account"));
    setAttribute(Qt::WA_DeleteOnClose, true);

    QWidget *widget = new QWidget(this);
    setupUi(widget);
    setMainWidget(widget);

    if (uploader && !account.isEmpty()) {
        accountEdit->setText(account);
        saveCheck->setChecked(hasPassword(account));
        m_account = account;
        if (saveCheck->isChecked()) {
            connect(uploader, SIGNAL(gotPassword(QString,QString)), this,
                    SLOT(gotPassword(QString,QString)));
            uploader->getPassword(account);
        }
    }

    connect(this, SIGNAL(finished(int)), this, SLOT(dialogFinished(int)));

}


AddAccountDialog::~AddAccountDialog()
{



}


void AddAccountDialog::removeAccount(const QString &account)
{

    KConfig *cfg = Settings::self()->config();
    KConfigGroup group(cfg, "youtube_accounts");
    QStringList accounts = group.readEntry("Accounts", QStringList());
    accounts.removeAll(account);

    group.deleteEntry(account);
    group.writeEntry("Accounts", accounts);

}


QStringList AddAccountDialog::getAccounts()
{

    KConfig *cfg = Settings::self()->config();
    KConfigGroup group(cfg, "youtube_accounts");
    return group.readEntry("Accounts", QStringList());

}


bool AddAccountDialog::hasPassword(const QString &account)
{

    KConfig *cfg = Settings::self()->config();
    KConfigGroup group(cfg, "youtube_accounts");
    return group.readEntry(account, false);

}


void AddAccountDialog::dialogFinished(const int &code)
{

    if (code == KDialog::Accepted) {
        const QString account = accountEdit->text();
        const QString password = passwordEdit->text();
        const bool savePassword = saveCheck->isChecked();

        KConfig *cfg = Settings::self()->config();
        KConfigGroup group(cfg, "youtube_accounts");
        QStringList accounts = group.readEntry("Accounts", QStringList());

        if (!m_account.isEmpty()) {
            accounts.removeAll(m_account);
            removeAccount(m_account);
        }

        if (!accounts.contains(account)) {
            accounts.append(account);
        }

        group.writeEntry(account, savePassword);
        group.writeEntry("Accounts", accounts);

        if (savePassword) {
            emit newPassword(account, password);
        }

        emit accountsChanged(accounts);
    }

}


void AddAccountDialog::gotPassword(const QString &account, const QString &password)
{

    if (accountEdit->text() == account) {
        passwordEdit->setText(password);
    }

}

