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


#ifndef RECORDITNOW_LISTLAYOUTROW_H
#define RECORDITNOW_LISTLAYOUTROW_H

// Qt
#include <QtGui/QFrame>


class QToolButton;
namespace RecordItNow {

    
class ListLayoutRow: public QFrame
{
    Q_OBJECT
    
public:
    explicit ListLayoutRow(QWidget *widget, QWidget *parent = 0, const bool &moveEnabled = false);
    
    QWidget *widget() const;
    void removeWidget();
    void setUpEnabled(const bool &enabled);
    void setDownEnabled(const bool &enabled);
    
    
private:
    QWidget *m_widget;
    QToolButton *m_upButton;
    QToolButton *m_downButton;
    
    
private slots:
    void removeClicked();
    void upClicked();
    void downClicked();

    
signals:
    void removeRequested(RecordItNow::ListLayoutRow *self);
    void upRequested(RecordItNow::ListLayoutRow *self);
    void downRequested(RecordItNow::ListLayoutRow *self);
    
    
};


} // namespace RecordItNow


#endif // RECORDITNOW_LISTLAYOUTROW_H
