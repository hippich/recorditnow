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


#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H


// own
#include "ui_settings.h"

// KDE
#include <kconfigdialog.h>


class RecordItNowPluginManager;
class KPluginSelector;
class ConfigDialog : public KConfigDialog
{
    Q_OBJECT


public:
    ConfigDialog(QWidget *parent, RecordItNowPluginManager *manager);
    ~ConfigDialog();


private:
    RecordItNowPluginManager *m_pluginManager;
    Ui::Settings ui_settings;
    KPluginSelector *m_pluginSelector;

    void init();


private slots:
    void updateEncoderCombo();
    void configFinished(const int &code);
    void pluginSettingsChanged(const bool &changed);
    void encoderChanged();


signals:
    void dialogFinished();


};


#endif // CONFIGDIALOG_H