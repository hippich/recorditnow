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


#ifndef SNOOPDIALOG_H
#define SNOOPDIALOG_H


// own
#include "ui_snoopdialog.h"

// KDE
#include <kdialog.h>


class SNoopDialog : public KDialog, public Ui::SNoopDialog
{
    Q_OBJECT


public:
    explicit SNoopDialog(QWidget *parent = 0);
    ~SNoopDialog();


private slots:
    void dialogFinished(const int &ret);
    void loadDeviceList();
    void loadDeviceList2();


signals:
    void deviceSelected(const QString &id);


};


#endif // SNOOPDIALOG_H
