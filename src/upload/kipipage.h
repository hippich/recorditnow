/***************************************************************************
 *   Copyright (C) 2010 by Kai Dombrowe <just89@gmx.de>                    *
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

#ifndef RECORDITNOW_KIPIPAGE_H
#define RECORDITNOW_KIPIPAGE_H


// own
#include "ui_kipipage.h"

// Qt
#include <QtGui/QWizardPage>


namespace KIPI {
    class Interface;
};


class KAction;
namespace RecordItNow {


class KIPIInterface;
class KIPIPage : public QWizardPage, Ui::KIPIPage
{
    Q_OBJECT


public:
    explicit KIPIPage(KIPI::Interface *interface, QWidget *parent = 0);
    ~KIPIPage();

    bool isComplete() const;

    void initializePage();


private:
    RecordItNow::KIPIInterface *m_interface;
    KAction *m_plugin;

    void setPlugin(KAction *plugin);


private slots:
    void currentSelectionChanged();


};


} // namespace RecordItNow


#endif // RECORDITNOW_KIPIPAGE_H
