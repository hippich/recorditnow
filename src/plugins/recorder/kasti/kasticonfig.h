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

#ifndef KASTICONFIG_H
#define KASTICONFIG_H


// own
#include "ui_kasticonfig.h"

// KDE
#include <kcmodule.h>


class KastiConfig : public KCModule, Ui::KastiConfig
{
    Q_OBJECT
    
public:
    explicit KastiConfig(QWidget *parent = 0, const QVariantList &args = QVariantList());
    ~KastiConfig();

    void load();
    void save();
    void defaults();
    
    
    
};


#endif // KASTICONFIG_H
