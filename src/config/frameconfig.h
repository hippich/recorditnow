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

#ifndef FRAMECONFIG_H
#define FRAMECONFIG_H


// own
#include "ui_frameconfig.h"
#include "config/configpage.h"
#include "frame/framesize.h"

// Qt
#include <QtGui/QWidget>


namespace RecordItNow {
    class ListLayout;
};


class KIntNumInput;
class QTreeWidgetItem;
class SizeWidget;
class FrameConfig : public RecordItNow::ConfigPage, Ui::FrameConfig
{
    Q_OBJECT


public:
    explicit FrameConfig(KConfig *cfg, QWidget *parent = 0);

    QList<FrameSize> sizes() const;
    static QList<FrameSize> defaultSizes();
    static QList<FrameSize> readSizes(KConfig *config);
    static void writeSizes(const QList<FrameSize> &sizes, KConfig *config);


public slots:
    void saveConfig();
    void setDefaults();
    void loadConfig();


private slots:
    void add();
    void remove(QWidget *widget);
    void textChanged(const QString &text);
    void itemChanged();


private:
    RecordItNow::ListLayout *m_layout;
    

};


#endif // FRAMECONFIG_H
