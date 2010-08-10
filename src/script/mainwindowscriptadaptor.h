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

#ifndef RECORDITNOW_MAINWINDOWSCRIPTADAPTOR_H
#define RECORDITNOW_MAINWINDOWSCRIPTADAPTOR_H


// own
#include "mainwindow.h"

// Qt
#include <QtCore/QObject>


namespace RecordItNow {


class MainWindow;
class MainWindowScriptAdaptor : public QObject
{
    Q_OBJECT


public:
    explicit MainWindowScriptAdaptor(QObject *parent);
    ~MainWindowScriptAdaptor();


public slots:
    void addToolWidget(QWidget *widget);
    void removeToolWidget(QWidget *widget);
    void addToolBarAction(QAction *action);
    void removeToolBarAction(QAction *action);


private slots:
    void stateChanged(const RecordItNow::MainWindow::State &state, const RecordItNow::MainWindow::State &oldState);


signals:
    void recordStarted();
    void recordPaused();
    void recordResumed();
    void recordFinished();


};


} // namespace RecordItNow


#endif // MAINWINDOWSCRIPTADAPTOR_H
