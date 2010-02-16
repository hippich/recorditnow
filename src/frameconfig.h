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

// Qt
#include <QtGui/QWidget>


class KIntNumInput;
class QTreeWidgetItem;
class SizeWidget;
class FrameConfig : public QWidget, public Ui::FrameConfig
{
    Q_OBJECT


public:
    explicit FrameConfig(const QList<QPair<QString,QSize> > &sizes, QWidget *parent = 0);

    QList< QPair<QString, QSize> > sizes() const;


private slots:
    void add();
    void remove();
    void up();
    void down();
    void itemSelectionChanged();
    void textChanged(const QString &text);
    void itemChanged(QTreeWidgetItem *item, int column);


private:
    KIntNumInput *newSizeWidget();

    void move(const int &from, const int &to);


signals:
    void configChanged();


};


#endif // FRAMECONFIG_H
