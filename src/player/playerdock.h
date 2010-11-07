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

#ifndef PLAYERDOCK_H
#define PLAYERDOCK_H


// own
#include "ui_playerdock.h"
#include "../dockwidget.h"

// Qt
#include <QtGui/QDockWidget>


class QSplitter;
class QStackedLayout;
namespace RecordItNow {


class CollectionView;
class CollectionItem;
class AbstractPlayer;
class PlayerDock : public RecordItNow::DockWidget, Ui::PlayerDock
{
    Q_OBJECT


public:
    explicit PlayerDock(QWidget *parent = 0);
    ~PlayerDock();

    bool play(const QString &file);


private:
    QList<AbstractPlayer*> m_playerWidgets;
    QStackedLayout *m_layout;
    CollectionView *m_view;
    QSplitter *m_splitter;

    void save();
    void load();


private slots:
    void changePlayer();
    void currentChanged();
    void playRequested(RecordItNow::CollectionItem *item);


};


} // namespace RecordItNow


#endif // PLAYERDOCK_H
