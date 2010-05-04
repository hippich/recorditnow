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

#ifndef RECORDITNOW_DOCKWIDGET_H
#define RECORDITNOW_DOCKWIDGET_H


// Qt
#include <QtGui/QDockWidget>


class QToolButton;
namespace RecordItNow {

    
class DockWidget : public QDockWidget
{
    Q_OBJECT
  
  
public:
    DockWidget(QWidget *parent);
    ~DockWidget();

    void setTitleBarWidget(QWidget *widget);
    void setWidget(QWidget *widget);
  
    QWidget *widget() const;
    QWidget *titleBarWidget() const;
  
    void setDockTitle(const QString &title);
    
  
public slots:
    void embed();


private:
    QWidget *m_widget;
    QWidget *m_title;
    QWidget *m_window;
    QToolButton *m_embedButton;
    QString m_dockTitle;
    
    
protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void closeEvent(QCloseEvent *event);
    
    
};


}


#endif // RECORDITNOW_DOCKWIDGET_H
