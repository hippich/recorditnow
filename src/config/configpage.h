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


#ifndef CONFIGPAGE_H
#define CONFIGPAGE_H


// Qt
#include <QtGui/QWidget>


class KConfig;
namespace RecordItNow {


class ConfigPage : public QWidget
{
    Q_OBJECT


public:
    explicit ConfigPage(KConfig *cfg, QWidget *parent = 0);
    ~ConfigPage();

    bool hasChanged();


public slots:
    void save();
    void defaults();
    void load();


private:
    KConfig *m_config;
    bool m_settingsChanged;


private slots:
    void configChangedInternal(const bool &changed);


protected:
    KConfig *config() const;

    virtual void saveConfig() {};
    virtual void setDefaults() {};
    virtual void loadConfig() {};

    Q_SIGNAL void configChanged(const bool &changed = true);


signals:
    void settingsChanged();


};


} // namespace RecordItNow


#endif // CONFIGPAGE_H
