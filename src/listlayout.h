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

#ifndef RECORDITNOW_LISTLAYOUT_H
#define RECORDITNOW_LISTLAYOUT_H


// Qt
#include <QtGui/QVBoxLayout>


namespace RecordItNow {

    
class ListLayoutRow;
class ListLayout: public QVBoxLayout
{
    Q_OBJECT
    
    
public:
    explicit ListLayout(QWidget *parent = 0, const bool &moveEnabled = false);
    ~ListLayout();    
    
    QList<QWidget*> rows() const;
    QWidget *take(QWidget *widget);    
    
    void addRow(QWidget *widget);
    void insertRow(QWidget *widget, const int &index);
    void removeRow(QWidget *widget);
    void clear();
    
    
private:
    QWidget *m_spacer;
    bool m_moveEnabled;
    
    void updateButtons();
    void move(const int &from, const int &to);
    
    
private slots:
    void removeClicked(RecordItNow::ListLayoutRow *row);
    void upRequested(RecordItNow::ListLayoutRow *row);
    void downRequested(RecordItNow::ListLayoutRow *row);
    
    
signals:
    void removeRequested(QWidget *row);
    void layoutChanged();
    

};


}

#endif // RECORDITNOW_LISTLAYOUT_H
