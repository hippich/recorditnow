/***************************************************************************
 *   Copyright (C) 210 by Kai Dombrowe <just89@gmx.de>                     *
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

#ifndef SOUNDROW_H
#define SOUNDROW_H


// own
#include "mousebuttonwidget.h"

// Qt
#include <QtGui/QWidget>


class KUrlRequester;
class SoundRow: public QWidget
{
    Q_OBJECT
    
    
public:
    SoundRow(QWidget *parent = 0);

    
    QString sound() const;
    int code() const;
    MouseButtonWidget::Button button() const;
    

    void setSound(const QString &sound);
    void setButton(const MouseButtonWidget::Button &button);

    
private:
    KUrlRequester *m_requester;
    MouseButtonWidget *m_button;
    
    
private slots:
    void playClicked();
    
        
signals:
    void removeRequested();
    void changed();
    void buttonChanged(const MouseButtonWidget::Button &oldButton, const MouseButtonWidget::Button &newButton);


};


#endif // SOUNDROW_H
