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


// own
#include "mainwindowscriptadaptor.h"
#include "helper.h"

// KDE
#include <kdebug.h>
#include <ktoolbar.h>


namespace RecordItNow {


MainWindowScriptAdaptor::MainWindowScriptAdaptor(QObject *parent)
    : QObject(parent)
{

    connect(Helper::self()->window(), SIGNAL(recordStateChanged(RecordItNow::MainWindow::State, RecordItNow::MainWindow::State)),
            this, SLOT(stateChanged(RecordItNow::MainWindow::State, RecordItNow::MainWindow::State)));

}


MainWindowScriptAdaptor::~MainWindowScriptAdaptor()
{



}


QToolBar *MainWindowScriptAdaptor::toolBar(const QString &name) const
{

    return Helper::self()->window()->toolBar(name);

}


void MainWindowScriptAdaptor::addToolWidget(QWidget *widget)
{

    Helper::self()->window()->scriptLayout->addWidget(widget);

}


void MainWindowScriptAdaptor::removeToolWidget(QWidget *widget)
{

    Helper::self()->window()->scriptLayout->removeWidget(widget);

}


void MainWindowScriptAdaptor::addToolBarAction(QAction *action)
{

    Helper::self()->window()->toolBar()->addAction(action);

}


void MainWindowScriptAdaptor::removeToolBarAction(QAction *action)
{

    Helper::self()->window()->toolBar()->removeAction(action);

}


void MainWindowScriptAdaptor::addDockWidget(const Qt::DockWidgetArea &area, QDockWidget *dock)
{

    Helper::self()->window()->addDockWidget(area, dock);

}


void MainWindowScriptAdaptor::removeDockWidget(QDockWidget *dock)
{

    Helper::self()->window()->removeDockWidget(dock);

}


void MainWindowScriptAdaptor::addToolBar(const Qt::ToolBarArea &area, QToolBar *bar)
{

    Helper::self()->window()->addToolBar(area, bar);

}


void MainWindowScriptAdaptor::removeToolBar(QToolBar *bar)
{

    Helper::self()->window()->removeToolBar(bar);

}


void MainWindowScriptAdaptor::stateChanged(const RecordItNow::MainWindow::State &state, const RecordItNow::MainWindow::State &oldState)
{

    switch (state) {
    case RecordItNow::MainWindow::Idle: emit recordFinished(); break;
    case RecordItNow::MainWindow::Paused: emit recordPaused(); break;
    case RecordItNow::MainWindow::Encode: emit recordFinished(); break;
    case RecordItNow::MainWindow::Recording: {
            switch (oldState) {
            case RecordItNow::MainWindow::Timer:
            case RecordItNow::MainWindow::Idle: emit recordStarted(); break;
            case RecordItNow::MainWindow::Paused: emit recordResumed(); break;
            default: break;
            }
        }
        default: break;
    }

}


} // namespace RecordItNow
